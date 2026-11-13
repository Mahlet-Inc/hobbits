#!/usr/bin/env bash
set -euo pipefail

check_dir=$(mktemp -d "${RUNNER_TEMP:-/tmp}/hobbits-package.XXXXXX")
trap 'rm -rf "$check_dir"' EXIT

if [[ "$1" == standalone ]]; then
    archives=(build/app/hobbits-*.tar.gz)
    test "${#archives[@]}" -eq 1
    mkdir "$check_dir/relocated package"
    tar -xf "${archives[0]}" -C "$check_dir/relocated package"
    mapfile -t runners < <(find "$check_dir/relocated package" -name hobbits-runner -type f)
    test "${#runners[@]}" -eq 1
    bin_dir=$(dirname "${runners[0]}")
    cp build/bin/hobbits-python-tests "$bin_dir/"
    # Test executables normally have build-tree RPATHs. Test only packaged libs.
    patchelf --set-rpath '$ORIGIN/../lib' "$bin_dir/hobbits-python-tests"
    test -f "$bin_dir/../lib/hobbits/python/lib/python3.14/encodings/__init__.py"
    test ! -d "$bin_dir/../lib/hobbits/python/lib/python3.14/site-packages"
    clean_env=(env -u LD_LIBRARY_PATH PATH=/nonexistent PYTHONHOME=/missing-python-home
        PYTHONPATH=/missing-python-path QT_QPA_PLATFORM=offscreen)
    "${clean_env[@]}" /usr/bin/timeout 30 "$bin_dir/hobbits-python-tests" --bundled
    "${clean_env[@]}" /usr/bin/timeout 30 "$bin_dir/hobbits-runner" list \
        --config "$check_dir/hobbits.ini" > "$check_dir/plugins.log" 2>&1
    cat "$check_dir/plugins.log"
    grep -qx 'Operator: Python Runner' "$check_dir/plugins.log"
    grep -qx 'Analyzer: Kaitai Struct' "$check_dir/plugins.log"
    ! grep -q 'could not be loaded as a plugin' "$check_dir/plugins.log"
elif [[ "$1" == deb ]]; then
    packages=(build/app/hobbits_*.deb)
    test "${#packages[@]}" -eq 1
    depends=$(dpkg-deb --field "${packages[0]}" Depends)
    printf 'Debian dependencies: %s\n' "$depends"
    [[ "$depends" == *python3.12* && "$depends" == *libpython3.12t64* ]]
    [[ "$depends" != *python3.14* ]]
    dpkg-deb --contents "${packages[0]}" > "$check_dir/contents"
    if grep -E '/libpython[0-9]|/python3[.][0-9]+/|/hobbits/python/' "$check_dir/contents"; then
        echo 'The Debian package must use system Python, not bundle a runtime' >&2
        exit 1
    fi
    cp build/bin/hobbits-python-tests "$check_dir/"
    patchelf --set-rpath /usr/lib/hobbits/lib "$check_dir/hobbits-python-tests"
    # A clean Ubuntu image proves apt pulls in Python and all other dependencies,
    # without access to GitHub's tool cache or this job's build outputs.
    docker run --rm -v "$PWD/build/app:/packages:ro" -v "$check_dir:/checks:ro" \
        -e QT_QPA_PLATFORM=offscreen ubuntu:24.04 bash -ec '
            apt-get update -qq
            apt-get install -y --no-install-recommends /packages/hobbits_*.deb
            timeout 30 /checks/hobbits-python-tests --system-python
            timeout 30 hobbits-runner list --config /tmp/hobbits.ini > /tmp/plugins.log 2>&1
            cat /tmp/plugins.log
            grep -qx "Operator: Python Runner" /tmp/plugins.log
            grep -qx "Analyzer: Kaitai Struct" /tmp/plugins.log
            ! grep -q "could not be loaded as a plugin" /tmp/plugins.log
        '
else
    echo "Unknown package flavor: $1" >&2
    exit 1
fi
