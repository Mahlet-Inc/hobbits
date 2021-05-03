# Python Plugin Developer Guide

Python plugins let you rapidly extend hobbits processing abilities with Python
code. Hobbits has a built-in interpreter, and a Python API that allows you to
process data at near-native speeds while still taking full advantage of the
flexibility and power of Python.

## Basic Plugin Structure

Plugins are broken into 2 main parts:

 - A JSON file describing the plugin
 - A Python script implementing the described plugin

## Plugin Installation

Python plugins can be installed in `python_displays`, `python_analyzers`,
`python_operators`, or `python_importers` folders wherever hobbits searches for
plugins (e.g. `~/.local/share/hobbits/plugins` or the `plugins` folder adjacent
to the hobbits binary.) For example, you could add display plugin files like:
```
~/.local/share/hobbits/plugins/python_displays/MyPyDisplay/plugin.json
~/.local/share/hobbits/plugins/python_displays/MyPyDisplay/plugin.py
```

### JSON File Structure

The JSON file should provide the following fields:

 - `name` - the name of the plugin (string)
 - `description` - a brief description of the plugin (string)
 - `tags` - tags for categorizing the plugin (array of strings)
 - `script` - the name of the python file with the implementation code (string)
 - `type` - the type of the plugin (`"importer"`, `"operator"`, or `"analyzer"`)
 - `extra_paths` - directories on the local machine where supporting python
 files/libraries can be found (array of strings)
 - `parameters` - the parameters that are required by the plugin. It is an
 array of parameter objects, each of which contains a `name` field (string)
 and a `type` field (`"string"`, `"integer"`, `"decimal"`, or `"boolean"`).
 Optionally, a parameter object can list all valid values in an array in a
 `possible_values` field, or, in the case of `"integer"` and `"decimal"` types,
 specify minimum and maximum values in the `min` and `max` fields. 

Here's an example JSON file:
```
{
    "name": "String Importer",
    "description": "Simple example string importer",
    "tags": ["Example"],
    "script": "plugin.py",
    "type": "importer",
    "extra_paths": [],
    "parameters": [
        {
            "name": "my_string",
            "type": "string"
        }
    ]
}
```

Here's a more complicated parameter list:
```
"parameters": [
    {
        "name": "hero",
        "type": "string",
        "possible_values": [
            "Sam",
            "Frodo",
            "Merry",
            "Pippin",
            "Bilbo
        ]
    },
    {
        "name": "breakfast_count",
        "type": "integer",
        "min": 2,
        "max": 10
    }
]
```

Display plugins can optionally set their `render_config` (default values shown):
```
{
    ...
    "type": "display",
    "render_config": {
        "asynchronous": true,
        "hide_bit_offset_controls": false,
        "hide_frame_offset_controls": false
    },
    ...
}
```


### Python Script Structure

The Python script must be valid Python, and it must define a valid entry point
function based on the JSON configuration of the plugin. For example, a valid
script for the "String Importer" above could be:
```
def import_bits(bits, info, my_string, progress):
    bits.set_bytes(0, my_string.encode('ascii'))
```

In general terms, a Python plugin with a type of `"importer"` must have a function
named `import_bits` that takes the following parameters (in order):

 - A `hobbits.BitArray` that will be the imported bits
 - A `hobbits.BitInfo` that will be the  imported bits info/metadata
 - A parameter for each parameter object specified in the `"parameters"` array of the JSON configuration
 - A `hobbits.ActionProgess` that can be used to update progress and check for cancellation

Similarly, an `"exporter"` type plugin must have a function named `export_bits`
that takes:

 - A `hobbits.ImmutableBitContainer` that will be exported
 - A parameter for each parameter object specified in the `"parameters"` array of the JSON configuration
 - A `hobbits.ActionProgess` that can be used to update progress and check for cancellation

Similarly, an `"analyzer"` type plugin must have a function named `analyze_bits`
that takes:

 - A `hobbits.ImmutableBitContainer` that will contain the bits and info to be analyzed
 - A `hobbits.BitInfo` that will be the new info for the bits post-analysis
 - A parameter for each parameter object specified in the `"parameters"` array of the JSON configuration
 - A `hobbits.ActionProgess` that can be used to update progress and check for cancellation

Similarly, an `"operator"` type plugin must have a function named `operate_on_bits`
that takes:

 - A `hobbits.ImmutableBitContainer` that will contain the bits and info to be operated on
 - A `hobbits.BitArray` that will be the output bits of the operation
 - A `hobbits.BitInfo` that will be the output bits info of the operation
 - A parameter for each parameter object specified in the `"parameters"` array of the JSON configuration
 - A `hobbits.ActionProgess` that can be used to update progress and check for cancellation

A `"display"` type plugin must have a function named `render_display`
that takes:

 - A `hobbits.DisplayHandle` that will contain the bit container and offsets
 - A `hobbits.ImageBuffer` that will have a size and should be filled with raw ARGB image data
 - A parameter for each parameter object specified in the `"parameters"` array of the JSON configuration
 - A `hobbits.ActionProgess` that can be used to update progress and check for cancellation

## Hobbits Python API

### hobbits.BitContainer

 - `BitContainer.bits` gets a `hobbits.ImmutableBitArray`
 - `BitContainer.info` gets a `hobbits.BitInfo`
 - `hobbits.ImmutableBitContainer` returns a `hobbits.ImmutableBitInfo` for the `info` property

### hobbits.BitArray

 - `BitArray.size` returns the size of the `BitArray` in bits
 - `BitArray.at(i)` returns the value of the `i`th bit
 - `BitArray.resize(n)` resizes the `BitArray` to length `n`
 - `BitArray.set(i, x)` sets the `i`th bit to boolean value `x`
 - `BitArray.read_bytes(i, n)` reads `n` bytes starting from the `i`th byte and returns the value as a Python `bytearray`
 - `BitArray.set_bytes(i, x)` writes bytes-like object `x` to the `BitArray` starting at the `i`th byte
 - `BitArray.write_to(fname)` writes the contents of the `BitArray` to a file named `fname`
 - `BitArray.read_from(i, fname)` reads the contents of the file named `fname` into the `BitArray` starting at the `BitArray`'s `i`th byte
 - `hobbits.ImmutableBitArray` only supports `size`, `at`, `read_bytes`, and `write_to`

### hobbits.BitInfo

 - `BitInfo.set_metadata(key, value)` sets the metadata `key` to `value` (`key` and `value` are both strings)
 - `BitInfo.get_metadata(key)` gets the metadata value of `key` if it is set (returns empty string if not set)
 - `BitInfo.add_highlight(category, label, start, end, color=0x44ff8800)` adds a RangeHighlight from bit `start` to `end`
 - `BitInfo.get_highlights(category)` gets a list of all RangeHighlights of category `category`

 - `hobbits.ImmutableBitInfo` only supports `get_metadata` and `get_highlights`

### hobbits.ActionProgess

 - `ActionProgess.is_cancelled()` returns true if the plugin action was cancelled and should be aborted
 - `ActionProgress.set_progress_percent(x)` reports the progress of the plugin action as `x` out of 100
 - `ActionProgress.set_progress(x, n)` reports the progress of the plugin action as `x` out of `n`

### hobbits.DisplayHandle

 - `DisplayHandle.current_container` the `BitContainer` that should be displayed
 - `DisplayHandle.bit_offset` the current bit offset 
 - `DisplayHandle.frame_offset` the current frame offset
 - `DisplayHandle.total_bit_offset` the total bit offset (start of offset frame + bit offset)

### hobbits.ImageBuffer

 - `ImageBuffer.set_bytes(x)` sets the image data to bytes-like object `x` (should be 4 * `width` * `height` long)
 - `ImageBuffer.width` width of the image (read-only)
 - `ImageBuffer.height` the height of the image (read-only)
