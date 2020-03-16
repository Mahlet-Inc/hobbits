import json
import sys
import math

from custom import Custom
from kaitaistruct import KaitaiStruct

def dumpStruct(s, sections, prefix=""):
    if isinstance(s, list):
        for i, item in enumerate(s):
            dumpStruct(item, sections, prefix + "[" + str(i) + "]")
    elif isinstance(s, KaitaiStruct):
        if hasattr(s, "_debug"):
            for name, descr in s._debug.items():
                prop = getattr(s, name)
                if not isinstance(prop, KaitaiStruct):
                    sections.append({
                    "start": descr["start"],
                    "end": descr["end"],
                    "label": f"{prefix}.{name}"
                    })
                    dumpStruct(prop, sections, prefix + "." + name)

if __name__ == "__main__":
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    target = Custom.from_file(input_filename)
    target._read()

    sections = []
    dumpStruct(target, sections)

    output_json = {
    "sections": sections
    }

    with open(output_filename, 'w') as output_file:
        json.dump(output_json, output_file)