#!/usr/bin/python3

import os
import sys

root_path = os.path.dirname(os.path.realpath(__file__))
git_dir = os.path.join(root_path, 'kaitai_struct_formats')
ksy_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../ksy')
ksy_py_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../ksy_py')

def clone_ksy_repo():
    remove_ksy_repo()
    import subprocess
    print(f'Cloning ksy repo...')
    p = subprocess.run(['git', 'clone', 'https://github.com/kaitai-io/kaitai_struct_formats.git', git_dir], capture_output=True, text=True)
    if 'fatal' in p.stdout or 'fatal' in p.stderr:
        print("Error:")
        print(p.stdout)
        print(p.stderr)
        exit(-1)


def remove_ksy_repo():
    import shutil
    if os.path.exists(git_dir):
        print(f'Removing ksy repo...')
        shutil.rmtree(git_dir)


def collect_useable_ksy_files():
    from shutil import copytree, ignore_patterns, rmtree
    omitted = ignore_patterns(
        ".*",
        "_build",
        "*.md",
        "**/nt_mdt.ksy", # incompatible license
        "lvm2.ksy", # incompatible license
        "vdi.ksy", # incompatible license
    )
    if os.path.exists(ksy_dir):
        print(f'Removing old ksy dir...')
        rmtree(ksy_dir)
    print(f'Copying ksy files..')
    copytree(git_dir, ksy_dir, ignore=omitted)

    if os.path.exists(ksy_py_dir):
        print(f'Removing old compiled ksy dir...')
        rmtree(ksy_py_dir)
    copytree(ksy_dir, ksy_py_dir)


def compile_ksy_files():
    print(f'Compiling ksy files..')
    ksy_files = []
    import subprocess
    for root, dirs, files in os.walk(ksy_py_dir):
        for f in files:
            path = os.path.realpath(os.path.join(root, f))
            d = os.path.dirname(path)
            print(f)
            p = subprocess.run([f'kaitai-struct-compiler --debug -t python --outdir "{d}" -I "{ksy_dir}:{d}" "{path}"'], capture_output=True, text=True, shell=True)
            if p.stderr:
                print("Kaitai Struct Compiler Error:")
                print(p.stderr)
            ksy_files.append(path)
    
    for f in ksy_files:
        os.remove(f)


def overwrite_resource_file():
    rcc_data = '<RCC><qresource prefix="/kaitaidata">\n'

    base_path = os.path.realpath(os.path.join(root_path, ".."))

    for root, dirs, files in os.walk(ksy_dir):
        for f in files:
            path = os.path.realpath(os.path.join(root, f))
            rel = os.path.relpath(path, base_path)
            rcc_data += f'  <file>{rel}</file>\n'

    for root, dirs, files in os.walk(ksy_py_dir):
        for f in files:
            path = os.path.realpath(os.path.join(root, f))
            rel = os.path.relpath(path, base_path)
            rcc_data += f'  <file>{rel}</file>\n'


    rcc_data += '</qresource></RCC>\n'
    with open(os.path.join(root_path, '../kaitaidata.qrc'), "w") as rcc:
        rcc.write(rcc_data)
        

if __name__ == "__main__":
    clone_ksy_repo()
    collect_useable_ksy_files()
    compile_ksy_files()
    overwrite_resource_file()
    remove_ksy_repo()