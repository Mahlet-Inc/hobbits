import json
import sys
import math
import importlib
import glob
import os

from kaitaistruct import KaitaiStruct

class OperationCancelled(Exception):
    """Raised when the operation is cancelled"""
    pass

class OperatorHandle:
    def __init__(self):
        pass

    def check_cancelled(self):
        if os.path.exists('abort'):
            raise OperationCancelled("Operation was cancelled")

    def set_progress(self, progress):
        with open('progress', 'w') as progress_file:
            json.dump({"progress": progress}, progress_file)

def dumpStruct(s, sections, prefix=""):
    if isinstance(s, list):
        for i, item in enumerate(s):
            label = prefix + "[" + str(i) + "]"
            sections.append({
                "label": label,
                "parent": prefix
            })
            dumpStruct(item, sections, label)
    elif isinstance(s, KaitaiStruct):
        if hasattr(s, "_debug"):
            for name, descr in s._debug.items():
                prop = getattr(s, name)
                if not isinstance(prop, KaitaiStruct):
                    label = prefix + "." + name if prefix else name
                    sections.append({
                        "start": descr["start"],
                        "end": descr["end"],
                        "label": label,
                        "parent": prefix
                    })
                    dumpStruct(prop, sections, label)

if __name__ == "__main__":
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    scripts = glob.glob('*.py')
    
    module_file = ""
    for script in scripts:
        if script != os.path.basename(__file__):
            module_file = script
            break
    package_name = os.path.splitext(module_file)[0]
    class_name = package_name.capitalize()
    struct_module = importlib.__import__(package_name, fromlist=[class_name])
    Struct = getattr(struct_module, class_name)

    operator_handle = OperatorHandle()
    operator_handle.check_cancelled()
    operator_handle.set_progress(5)

    target = Struct.from_file(input_filename)
    target._read()

    operator_handle.check_cancelled()
    operator_handle.set_progress(70)

    sections = []
    dumpStruct(target, sections)

    operator_handle.check_cancelled()
    operator_handle.set_progress(80)

    output_json = {
    "sections": sections
    }

    with open(output_filename, 'w') as output_file:
        json.dump(output_json, output_file)
