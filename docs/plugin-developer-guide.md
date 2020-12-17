# Hobbits Plugin Development

The Hobbits framework is designed to coordinate the capabilities of various
plugins into a smooth, cohesive experience. The plugins are where all of the
"actual work" gets done.

There are currently four types of plugins in Hobbits: Displays, Analyzers,
Operators, and Importer/Exporters. This document will cover general plugin
implementation concepts, and then walk through the development process for each
type of plugin.

## Example Plugin Development Video

![YOUTUBE](Dg3vknwLO74)

## General Terms and Concepts

### Qt Plugin API

Hobbits leverages the low-level plugin API provided by Qt for its plugin system.
Refer to the following Qt documentation for details:

 - [Low-level Plugin API](https://doc.qt.io/qt-5/plugins-howto.html#the-low-level-api-extending-qt-applications)
 - [Deploying Plugins](https://doc.qt.io/qt-5/deployment-plugins.html)

### Bit Containers

The `BitContainer` class is the primary means of storing data, and sharing it
between plugins. A `BitContainer` holds binary data, as well as metadata such as
how that data is framed. It also keeps a record of any changes that have been
made to it by Hobbits plugins.

The binary data is held in a `BitArray`, which abstracts byte boundaries from
the user, and has a modest-but-growing assortment of performance efficiencies
and helper functions.

Metadata is stored in a `BitInfo` object which is read and written with the
`bitInfo` and `setBitInfo` methods.

The framing of a bitstream is an important part of its metadata for display
and processing purposes. The frames of a `BitContainer` can be set in the
`BitInfo::setFrames` method.

General purpose metadata can be set with `BitInfo::setMetadata` and read with
`BitInfo::metadata`. Plugins
can use container metadata as a general purpose bus for communicating with other
plugins, or even as a cache for their own purposes. For example, the "Find"
Analyzer plugin stores the results of a find operation in a container's
metadata so that when that container loses focus, the information will still be
available when the container comes back into focus.

Highlights are a separate kind of metadata that deal with ranges of bits within
a bit container. They are read and written with various functions including
`BitInfo::addHighlights` and `BitInfo::highlights`
, and they are stored as a map of strings to `RangeHighlight` lists.
The "Find" Analyzer plugin
sets highlights in the ranges where it finds its search string, and several
Display plugins check for the existence and colors of `RangeHighlight`s in order
to render visual highlights across those ranges. Plugins like the `Extractor`
operator use `RangeHighlight`s for their processing.

Analyzer plugins do not have write access to bit containers, so they cannot edit
the `BitInfo` directly. As a result, they must include
a container's updated `BitInfo` in the
`AnalyzerResult`s that they return.

### Parameters, Delegates, and Editors

Most plugins require customizable parameters in order to perform their function.
Parameters are provided as `QJsonObject`s in the code so that they can be easily
serialized. Most plugins provide a `ParameterDelegate` that provides details and
validation capabilities for the JSON parameters, and optionally provides an
implementation of `AbstractParameterEditor` for editing the parameters in a GUI.
For most plugins, it is best to follow the pattern used in the Qt Creator plugin
templates that does the following:

 - Create Qt Designed Form class that implementes `AbstractParameterEditor`
 - Use a `ParameterHelper` to wrap each of the UI elements for easy
   `QJsonObject` getting and setting.
 - Emit `AbstractParameterEditor`'s `changed` signal whenever the UI changes.
 - If the editor needs to react to or enrich `BitContainer` metadata, implement
   `previewBitsUiImpl`. `previewBitsImpl` can also be used for off-thread
   pre-processing before `previewBitsUiImpl` is called.
 - In the plugin implementation, create a list of `ParameterInfo`s that describe
   the parameters.
 - In the plugin implementation, create the plugin's `ParameterDelegate` with
   `ParameterDelegate::create` using the `ParameterInfo` list, a `std::function`
   that describes the plugin operation based on some parameter set, and another
   `std::function` that returns the `AbstractParameterEditor` implementation.
   This is the `ParameterDelegate` that would then get returned in, for example,
   in `AnalyzerInterface::parameterDelegate`.

### Action Progress

The `ActionProgress` parameter is passed to some of the core plugin methods. It
allows a plugin to report its percentage progress as it executes, and allows it
to check if it has been cancelled so that it can halt execution gracefully. It
is possible for this parameter to be null, so plugins should check for that.

### Threads

Many of the main plugin methods will be executed on secondary threads, separate
from the the main Qt GUI thread. In those methods, it is important to avoid
using general Qt GUI functionality (e.g. creating a `QMessageBox` pop-up) or the
application might crash horribly. Read more about threads and Qt
[here](https://doc.qt.io/qt-5/thread-basics.html).

### Common Plugin Methods

Hobbits plugins have a base abstract interface `HobbitsPlugin` with the
following methods:

 - `name` returns a unique plugin name
 - `description` returns a description of the plugin
 - `tags` returns a list of categories that this plugin is affiliated with

There is also a common `createDefaultX` method (where X is the interface type)
that simply returns a default instance of the plugin.

## Specific Interfaces

### Operator Interface

Operators take some number of data inputs, and produce some number of data
outputs. Most operators are 1-in, 1-out, but the flexibility of the interface
enables several critical operations (e.g. data generators, muxes, demuxes.)
The output of operators will go into new containers so that the original data
can still be easily referenced.

`operateOnBits` takes a list of read-only bit containers, `QJsonObject`
parameters, and an `ActionProgress` instance. It returns an `OperatorResult`
which contains any new bit containers that have been created by the operator,
and the `QJsonObject` parameters that will enable the operation to be duplicated
exactly. This method may be executed on a secondary thread (see
[threads guidance above](#threads))

`getMinInputContainers` and `getMaxInputContainers` return the minimum and
maximum number of containers that the operator accepts as inputs. Operators that
take a single input and produce a single output would simply return 1 for both
of these functions.

### Analyzer Interface

Analyzers digest and decorate the data in a way that facilitates follow-on
processing and/or human evaluation.

`analyzeBits` takes a read-only bit container, `QJsonObject` parameters, and an
`ActionProgress` instance. It returns an `AnalyzerResult` which contains new
general metadata and range entries for the container, and the `QJsonObject`
parameters that will enable the operation to be duplicated exactly. This method
may be executed on a secondary thread (see [threads guidance above](#threads))

### Display Interface

Displays show the data in some sort of useful format (e.g. a bit raster, or a
hex dump.)  They can use metadata and highlights in bit containers to augment a
depiction of the data, but they could also show *only* the metadata if that was
useful somehow (e.g. if there were some metrics worth presenting separately from
the plugin that generated them.)

`renderConfig` returns a `DisplayRenderConfig` that provides some basic guidance
on how the display should be rendered, for example, whether or not it should be
rendered asynchronously on a secondary thread.

`setDisplayHandle` is how the plugin receives the shared `DisplayHandle` which
provides access to a variety of things that a display might need to access (e.g.
the current bit container), or set (e.g. the bit that is currently being hovered
over by the mouse.) It also lets the display set things like the currently
displayed range of bits with `DisplayHandle::setRenderedRange`.

`renderDisplay` is the primary display rendering function that takes the a
viewport size, `QJsonObject` parameters, and an `ActionProgress` instance. It
returns a `DisplayResult` which contains a `QImage` and the `QJsonObject`
parameters that will enable the operation to be duplicated exactly. This method
may be executed on a secondary thread (see [threads guidance above](#threads))

`renderOverlay` is similar to `renderDisplay`, but it does not receive an
`ActionProgress`. An overlay is generally something that might get adjusted more
frequently and separately from the main display, like a hover-over tooltip. It
generally is not executed on a secondary thread, but it can be (for example in a
`DisplayPrinter` exporter)

### Import/Export Interface

Import/Export plugins import and/or export bit containers for use in Hobbits.
A simple example of an ImporterExporter is the "File Data" plugin that
imports/exports raw binary data from/to files.

`canExport` and `canImport` return a boolean value indicating whether or not the
plugin is capable of importing or exporting (e.g. you might want a plugin that
can import data but not export it.) You can return `false` in both methods, but
that would be impolite.

`importBits` and `exportBits` both receive `QJsonObject` parameters, and an
`ActionProgress` instance.

`exportBits` also receives a bit container that should be exported. It returns
an `ExportResult` with the `QJsonObject` parameters that will enable the
operation to be duplicated exactly.

`importBits` returns a non-empty bit container in its `ImportResult `if it was
successful at importing data along with the `QJsonObject` parameters.

## Helpful Tools

In order to simplify the process of developing plugins, there are Qt Creator
wizards that take care of most of the boilerplate code.

The wizards can be installed using the wizard_installer.sh script.

## Get in touch

If you are having trouble making a plugin, or if you have a suggestion for
the hobbits API or documentation please open an issue on
[the GitHub page](https://github.com/Mahlet-Inc/hobbits/issues)
or ask about it on [the Discord channel](https://discord.gg/wRQJpZZ)
