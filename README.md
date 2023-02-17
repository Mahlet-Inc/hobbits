
[![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release)
[![Discord Chat](https://discordapp.com/api/guilds/672761400220844042/widget.png?style=shield)](https://discord.gg/wRQJpZZ)  

## About
Hobbits was developed at Mahlet in 2019 as an integrated data analysis tool. It was open-sourced
and is now (passively) maintained by [Adam](https://github.com/hello-adam) and anyone who wants
to contribute. Join the [Discord server](https://discord.gg/wRQJpZZ) if you want to ask questions,
contribute, or just chat about binary analysis tools.

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

## Building from source

1. Install the required build tools and development libraries.

Ubuntu 22.04 LTS example:
```
sudo apt-get install -y patchelf libusb-1.0-0-dev libpcap-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools python3-dev ninja-build cmake g++
```

2. Build with CMake
```
cmake -G Ninja -B build .
cmake --build build
```

If building using the bundled pffft, ensure you clone or checkout with the
`--recurse-submodules` flag.

## Adding plugins
Read the
[plugin developer guide](https://mahlet-inc.github.io/plugin-developer-guide/)
in the documentation.

![Screenshot of the Hobbits GUI](docs/hobbits_screenshot.png)
