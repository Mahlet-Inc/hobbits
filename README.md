
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

## Building with CMake 3.15+, Qt 5.12+, Python 3.9, FFTW 3.3.5+, Ninja, and libpcap
1. If you don't already have the Qt libraries and Qt Creator, you can get them from [qt.io/download-qt-installer](https://www.qt.io/download-qt-installer)
2. Install Python 3.9 with development libraries and headers (builds for
different systems can be found [here](https://dev.azure.com/mahlet-inc/hobbits/_build/results?buildId=488&view=artifacts&pathAsName=false&type=publishedArtifacts))
3. Install the FFTW 3.3.5 and libpcap development libraries
4. Configure and build "standalone" binaries with CMake:
```
mkdir build
cd build
cmake -GNinja -DMANUAL_PYTHON_PATH=<python root path> -DSELF_CONTAINED_APP=1 ..
ninja
ninja package
```

## Using Hobbits
Read the [user guide](https://mahlet-inc.github.io/user-guide/) in the
documentation and/or watch
[this walkthrough video](https://youtu.be/6ygkhze36qM)

## Adding plugins
```
Notice (November 2020): Several plugin interfaces are currently being refactored
and much of the documentation is out of date by a few minor version numbers. The
documentation will be updated as the 1.0 interface is decided upon.

Update (December 2020): most of the text documentation is updated, but videos
and screenshots are still lagging a little behind.

Update (March 2021): My goal is to have the documentation and API stabilized
around June 2021.
```
1. Read the
[plugin developer guide](https://mahlet-inc.github.io/plugin-developer-guide/)
in the documentation and/or watch
[this plugin development video](https://youtu.be/Dg3vknwLO74)
2. Run [wizard_installer.sh](wizards/wizard_installer.sh) from inside the
[wizards](wizards) directory to add Hobbits Plugin Qt Creator templates (restart
Qt Creator after doing this)
3. Add a plugin subproject to the appropriate plugins project and implement your
desired functionality

## Get Involved
Join our [Discord server](https://discord.gg/wRQJpZZ), send us an
email at opensource@mahletconsulting.com, and tell your colleagues. We'd love
contributions and resumes, but we'll settle for bug reports and memes.


![Screenshot of the Hobbits GUI](docs/hobbits_screenshot.png)
