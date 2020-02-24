
[![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release)
[![Discord Chat](https://img.shields.io/discord/308323056592486420.svg)](https://discord.gg/wRQJpZZ)  

| master | develop |
| ------ | ------- |
|[![Master Build Status](https://dev.azure.com/mahlet-inc/hobbits/_apis/build/status/Mahlet-Inc.hobbits?branchName=master)](https://dev.azure.com/mahlet-inc/hobbits/_build/latest?definitionId=1&branchName=master)|[![Develop Build Status](https://dev.azure.com/mahlet-inc/hobbits/_apis/build/status/Mahlet-Inc.hobbits?branchName=develop)](https://dev.azure.com/mahlet-inc/hobbits/_build/latest?definitionId=1&branchName=develop)|

## Documentation
The documentation is available at
[mahlet-inc.github.io](https://mahlet-inc.github.io).

## Pre-built binaries
There are pre-built binaries of the latest release on the
[GitHub releases page](https://github.com/Mahlet-Inc/hobbits/releases).

## Building with Qt Creator and Qt 5.13+
1. (Optional) Install any dev libraries required to get the full functionality
of the core plugins ([fftw3](http://www.fftw.org/download.html))
2. Open the root project file, [src/hobbits.pro](src/hobbits.pro), in Qt Creator
3. Build and run the application

## Using Hobbits
Read the [user guide](https://mahlet-inc.github.io/user-guide/) in the
documentation and/or watch
[this walkthrough video](https://youtu.be/6ygkhze36qM)

## Adding plugins
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