
[![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release)
[![Discord Chat](https://discordapp.com/api/guilds/672761400220844042/widget.png?style=shield)](https://discord.gg/wRQJpZZ)  

| master | develop |
| ------ | ------- |
|[![Master Build Status](https://dev.azure.com/mahlet-inc/hobbits/_apis/build/status/Mahlet-Inc.hobbits?branchName=master)](https://dev.azure.com/mahlet-inc/hobbits/_build/latest?definitionId=1&branchName=master)|[![Develop Build Status](https://dev.azure.com/mahlet-inc/hobbits/_apis/build/status/Mahlet-Inc.hobbits?branchName=develop)](https://dev.azure.com/mahlet-inc/hobbits/_build/latest?definitionId=1&branchName=develop)|


## Documentation
The documentation is available at
[mahlet-inc.github.io](https://mahlet-inc.github.io).

## Pre-built binaries
There are pre-built binaries of the latest release on the
[GitHub releases page](https://github.com/Mahlet-Inc/hobbits/releases).

## Using Hobbits
Read the [user guide](https://mahlet-inc.github.io/user-guide/) in the
documentation and/or watch
[this walkthrough video](https://youtu.be/6ygkhze36qM)

## Building with Conan, Python 3, and CMake 3.12+
1. Install Conan with pip: `pip install conan`

2. Configure Conan to use a custom remote:
```
conan remote add hobbitsconan https://helloadam.jfrog.io/artifactory/api/conan/conan
```

3. You might need some system libraries and build tools for all of the conan steps to work.

Ubuntu 20.04 example:
```
sudo apt-get install -y libudev-dev libgl-dev patchelf
```
Fedora 36 example:
```
sudo dnf install perl libglvnd-devel systemd-devel patchelf
```

4. You might need to add conan to your path and set the `CONAN_SYSREQUIRES_MODE` variable to `enabled`. Ubuntu 20.04 example (this could go in your .bashrc):
```
export PATH=/home/<your username>/.local/bin:$PATH
export CONAN_SYSREQUIRES_MODE=enabled
```

5. Use conan to get/build dependencies, then build and package hobbits:
```
mkdir build
cd build
conan install .. --build=missing
conan build ..
conan package ..
```

If you have any issues building, you can check out more detailed steps and
environmental setup in the [continuous integration files](ci) or
[open an issue](https://github.com/Mahlet-Inc/hobbits/issues/new).

## Building with CMake and system dependencies

Plain CMake (no Conan) building also works if you have your dependencies
handled. The [CentOS 7 GCC 4.8 CI build](ci/linux-cmake.yml) does this.
If building using the bundled pffft, ensure you clone or checkout with the
`--recurse-submodules` flag.

## Adding plugins
Read the
[plugin developer guide](https://mahlet-inc.github.io/plugin-developer-guide/)
in the documentation. As of v0.50.0, the plugin interfaces are pretty stable,
but some planned refactoring efforts will change parts of it before 1.x is
finalized.

## Get Involved
Join our [Discord server](https://discord.gg/wRQJpZZ), send us an
email at opensource@mahletconsulting.com, and tell your colleagues. We'd love
contributions and resumes, but we'll settle for bug reports and memes.


![Screenshot of the Hobbits GUI](docs/hobbits_screenshot.png)
