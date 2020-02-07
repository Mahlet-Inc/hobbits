# Hobbits Plugin Development

The Hobbits framework is designed to coordinate the capabilities of various
plugins into a smooth, cohesive experience. The plugins are where all of the
"actual work" gets done.

There are currently three types of plugins in Hobbits: Displays, Analyzers, and
Operators. This document will cover general plugin implementation concepts, and
then walk through the development process for each type of plugin.

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

General metadata is read and written with the `getMetadata` and `setMetadata`
methods, and it is stored as a simple map of strings to string lists. Plugins
can use container metadata as a general purpose bus for communicating with other
plugins, or even as a cache for their own purposes. For example, the "Find"
Analyzer plugin stores the results of a find operation in a container's
metadata so that when that container loses focus, the information will still be
available when the container comes back into focus.

Highlights are a separate kind of metadata that deal with ranges of bits within
a bit container. They are read and written with the `getHighlights` and
`setHighlights` methods, and they are stored as a map of strings to range lists
(a range list is a list of start and end indices.) The "Find" Analyzer plugin
sets highlights in the ranges where it finds its search string, and several
Display plugins check for the existence of highlights with the "find" or
"find-focus"keys, and render visual highlights over those sections.

Analyzer plugins do not have write access to bit containers, so they cannot use
the `setMetadata`, and `setHighlights` methods. As a result, they must include
the metadata and highlight entries they wish to add to the container in the
`AnalyzerResult`s that they return. The `AnalyzerResult` has `addRanges` and
`addMetadata` methods for this purpose.

**Also, in order for an analyzer to set the frames of a bit container, it must**
**return an `AnalyzerResult` with the desired frames set via `addRanges` with**
**the "frames" key, e.g. `result.addRanges("frames", frameRanges)`, and,**
**ideally, with the maximum frame width set as well via the general metadata,**
**e.g. `result.addMetadata("max_frame_width", maxWidth)`**. As a result of this
special behavior, the "frames" range key, and the "max_frame_width" metadata
key should be considered reserved, and used only for this situation.

### Common Plugin Interface Methods

All of the plugin interfaces have a `getName` method that must return the name
of that plugin as it should appear to the user. The name must be unique. There
is also a common `createDefaultX` method (where X is the interface type) that
simply returns a default instance of the plugin.

The Analyzer and Operator interfaces have several simple methods in common:

 - `applyToWidget` initializes the plugin's UI on a given widget
 - `provideCallback` gives the plugin a `PluginCallback` instance that can be
   used to request that it gets executed (this is useful because the central
   application that triggers plugin execution is decoupled from the plugin's UI,
   so in order for a plugin to trigger execution when, for example, the "Enter"
   key is pressed in its UI form, it needs to relay this request through the
   `PluginCallback` instance it is given.)
 - `getStateFromUi` gets the plugin's current state/configuration based on how
   its UI is filled out. If there are invalid or incomplete entries, the plugin
   can return an empty state to indicate that it is not ready for execution.
 - `canRecallPluginState` checks a plugin state to see if it can be used to
   execute the plugin. If the state is empty or incomplete, it must return
   false.

#### Plugin State

The plugin state contains parameters for how the plugin should run. For example,
the Take Skip operator plugin will require a "take_skip_string" parameter to
guide its execution. The state that it passed in can be provided by the plugin's
own `getStateFromUi` method, or it can come from a saved template that was
generated from a state returned by a plugin result (`OperatorResult` or
`AnalyzerResult`.) Plugin results will usually just return the state that was
passed in (e.g. `result->setPluginState(recallablePluginState)`,) but sometimes
that state needs to be enriched with execution details that were added by the
plugin, such as a random number seed, so that the execution can be duplicated
exactly with the returned state.

#### Action Progress

The `ActionProgress` parameter is passed to the primary methods of the operator
and analyzer interfaces. It allows a plugin to report its percentage progress as
it executes, and allows it to check if it has been cancelled so that it can halt
execution gracefully.

## Specific Interfaces

### Operator Interface

Operators take some number of data inputs, and produce some number of data
outputs. Most operators are 1-in, 1-out, but the flexibility of the interface
enables several critical operations (e.g. data generators, muxes, demuxes.)
The output of operators will go into new containers so that the original data
can still be easily referenced.

`operateOnContainers` takes a list of read-only bit containers, a plugin state,
and an `ActionProgress` instance. It returns an `OperatorResult` which contains
any new bit containers that have been created by the operator, and the plugin
state that will enable the operation to be duplicated exactly (see 
[plugin state details above](#plugin-state).) **`operateOnContainers` might run
in a separate thread from the main GUI thread, so you cannot use Qt features
that depend on being in the main thread, e.g., showing a QMessageBox.**

`getMinInputContainers` and `getMaxInputContainers` return the minimum and
maximum number of containers that the operator accepts as inputs. Operators that
take a single input and produce a single output would simply return 1 for both
of these functions.

### Analyzer Interface

Analyzers digest and decorate the data in a way that facilitates follow-on
processing and/or human evaluation.

`analyzeBits` takes a read-only bit container, a plugin state, and an
`ActionProgress` instance. It returns an `AnalyzerResult` which contains new
general metadata and range entries for the container, and the plugin
state that will enable the operation to be duplicated exactly (see 
[plugin state details above](#plugin-state).) **`analyzeBits` might run in a
separate thread from the main GUI thread, so you cannot use Qt features that
depend on being in the main thread, e.g., showing a QMessageBox.**

`previewBits` takes a read-only bit container and it can be used to prepare the
plugin for execution or enhance the UI options it presents. The Width Framer
analyzer plugins generates auto-correlation data in the `previewBits` method,
which is used to display a width-selector graph with suggested widths. It is
fine to leave this method's implementation empty.

### Display Interface

Displays show the data in some sort of useful format (e.g. a bit raster, or a
hex dump.)  They can use metadata and highlights in bit containers to augment a
depiction of the data, but they could also show *only* the metadata if that was
useful somehow (e.g. if there were some metrics worth presenting separately from
the plugin that generated them.)

`getDisplayWidget` and `getControlsWidget` provide the display and (optional)
controls for a display plugin. They both take a `DisplayHandle` as their sole
parameter.

The `DisplayHandle` provides access to a variety of things that a display might
need to access (e.g. the current bit container), or set (e.g. the bit that is
currently being hovered over by the mouse.)

### Import/Export Interface

Import/Export plugins import and/or export bit containers for use in Hobbits.
The simplest of these plugins is the "File Data" plugin that imports/exports
raw binary data from/to files.

`canExport` and `canImport` return a boolean value indicating whether or not the
plugin is capable of importing or exporting (e.g. you might want a plugin that
can import data but not export it.) You can return `false` in both methods, but
that would be impolite.

`importBits` and `exportBits` both receive a map of arguments (which might be
empty) and a pointer to a `QWidget` that should be used as the parent to any GUI
elements generated by the plugin (e.g. a file dialog.) 

`exportBits` also receives a bit container that should be exported.

`importBits` returns a non-empty bit container if it was successful at importing
data.

## Helpful Tools

In order to simplify the process of developing plugins, there are Qt Creator
wizards and CMake/Conan Python cookiecutters that take care of most of the
boilerplate code.

The wizards can be installed using the wizard_installer.sh script.