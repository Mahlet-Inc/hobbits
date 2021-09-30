import json
import sys
import math
import importlib
import glob
import os
import binascii
import traceback
from enum import Enum

from kaitaistruct import KaitaiStruct

import pprint
pp = pprint.PrettyPrinter(indent=2)


def process_value(value, section):

    if isinstance(value, bytes):
        if len(value) > 15:
            value = f"0x{binascii.hexlify(value[:12]).decode()}..."
        else:
            value = f"0x{binascii.hexlify(value).decode()}"
        section['value'] = value
        section['type'] = f"bytes[{len(value)}]"

    elif isinstance(value, str):
        if len(value) > 15:
            value = f"{value[:12]}..."
        section['value'] = value
        section['type'] = f"str[{len(value)}]"

    elif isinstance(value, float):
        section['value'] = value

    elif isinstance(value, int):
        section['value'] = value

    elif isinstance(value, bool):
        section['value'] = value

    elif isinstance(value, Enum):
        section['type'] = type(value).__name__
        section['value'] = value.name

    else:
        return False
    
    return True


def parse_struct(struct, sections, prefix="", parent_offset = 0, base_io=None, base_offset=0):
    if not isinstance(struct, KaitaiStruct):
        return
    
    print(f"Parsing {type(struct).__name__} at '{prefix}'")

    # iterate through members in order to read lazy instances
    for attr in vars(type(struct)):
        #print(f"Checking attribute {attr}")
        prop = getattr(type(struct), attr, None)
        if isinstance(prop, property):
            #print(f"Getting property {attr}")
            try:
                getattr(struct, attr)
            except:
                print(f"Failed when getting property {attr}: {traceback.format_exc()}")

    if not hasattr(struct, "_debug"):
        return
    
    
    section_io = struct._io
    if base_io is None:
        base_io = section_io
    elif section_io is not base_io:
        base_io = section_io
        base_offset = base_offset + parent_offset
        print(f"New base offset for {type(struct).__name__}: {base_offset}")
    
    for name, info in struct._debug.items():
        try:
            value = getattr(struct, name)
        except:
            print(f"Skipping {name}, not an attribute in struct")
            continue

        label = prefix + "." + name if prefix else name
        parent_offset = info["start"] + base_offset

        #print(f"{name}:")
        #pp.pprint(info)

        if not ("end" in info and "start" in info):
            print(f"Skipping {name}, no start-end...")
            continue

        section = {
            "start": info["start"] + base_offset,
            "end": info["end"] + base_offset,
            "label": label,
            "parent": prefix
        }


        if isinstance(value, KaitaiStruct):
            section['type'] = type(value).__name__
            sections.append(section)
            parse_struct(value, sections, label, parent_offset, base_io, base_offset)
        
        elif isinstance(value, list) and len(value) > 0:
            sections.append(section)
            for idx, value_item in enumerate(value):
                if not struct._debug[name] or not struct._debug[name]['arr'] or idx >= len(struct._debug[name]['arr']):
                    continue

                idx_label = f"{label}[{idx}]"
                idx_section = {
                    "label": idx_label,
                    "parent": label
                }

                idx_offset = struct._debug[name]['arr'][idx]['start'] + base_offset

                if isinstance(value_item, KaitaiStruct):
                    section['type'] = f"{type(value_item).__name__}[{len(value)}]"
                    sections.append(idx_section)
                    parse_struct(value_item, sections, idx_label, idx_offset, base_io, base_offset)
                else:
                    value_item_section = {
                        "start": idx_offset,
                        "end": struct._debug[name]['arr'][idx]['end'] + base_offset,
                        "label": idx_section["label"],
                        "parent": idx_section["parent"],
                    }
                    if process_value(value_item, value_item_section):
                        section['type'] = f"{value_item_section.get('type', 'array')}[{len(value)}]"
                        sections.append(value_item_section)
        
        elif process_value(value, section):
            sections.append(section)
        
        else:
            section["value"] = f"<? {str(value)[:50]} ?>"
            sections.append(section)
            


def parse_data(input_filename, output_filename, action_progress):
    # locate the compiled struct module
    scripts = glob.glob(os.path.join(os.path.dirname(input_filename), '*.py'))
    if len(scripts) < 1:
        raise FileNotFoundError('Could not find the expected python kaitai parser - did the kaitai struct compiler fail?')
    module_file = os.path.basename(scripts[0])
    sys.path.append(os.path.dirname(scripts[0]))
    package_name = os.path.splitext(module_file)[0]
    class_name = "".join([s.capitalize() for s in package_name.split("_")])
    try:
        del sys.modules[package_name]
    except KeyError:
        pass
    struct_module = importlib.__import__(package_name, fromlist=[class_name])
    Struct = getattr(struct_module, class_name)

    action_progress.set_progress_percent(5)

    # parse the input data
    target = Struct.from_file(input_filename)
    try:
        target._read()

        action_progress.set_progress_percent(70)

        # write the parser result to the output
        sections = []

        parse_struct(target, sections)
    finally:
        target._io.close()

    #pp.pprint(sections)

    action_progress.set_progress_percent(80)

    output_json = {
    "sections": sections
    }

    with open(output_filename, 'w') as output_file:
        json.dump(output_json, output_file)
