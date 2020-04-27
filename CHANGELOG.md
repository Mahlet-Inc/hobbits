# [0.29.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.28.0...v0.29.0) (2020-04-27)


### Features

* include file loading and presets in kaitai struct plugin ([f348af8](https://github.com/Mahlet-Inc/hobbits/commit/f348af8ee6e657c1ce7f9b2bcb6d110311ef7e9f))
* make some improvements to the width framer ([0dd2f73](https://github.com/Mahlet-Inc/hobbits/commit/0dd2f732f129957e9b8c2c9d01db775f64ffc969))

# [0.28.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.27.0...v0.28.0) (2020-04-27)


### Features

* add full file visual preview with navigation ([f61210b](https://github.com/Mahlet-Inc/hobbits/commit/f61210b0671db33ebd3dd513f5e0b3d91f3a87da))

# [0.27.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.26.0...v0.27.0) (2020-04-22)


### Features

* add fractal hilbert plot ([727a6f1](https://github.com/Mahlet-Inc/hobbits/commit/727a6f14bc14ea3f46e515a2dba9a2aac6e45d5b))

# [0.26.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.25.1...v0.26.0) (2020-04-21)


### Features

* add dot, frequency, and digraph plot display plugins ([272d692](https://github.com/Mahlet-Inc/hobbits/commit/272d692aa598ca605ba589c0171c790ba2e9c525))

## [0.25.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.25.0...v0.25.1) (2020-04-17)


### Bug Fixes

* export crash on null container and dialog window titles ([702c2ca](https://github.com/Mahlet-Inc/hobbits/commit/702c2ca1c8ea38d7599ee7248f16d6551246f455))

# [0.25.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.24.0...v0.25.0) (2020-04-10)


### Features

* add TCP stream import/export plugin ([384e9a8](https://github.com/Mahlet-Inc/hobbits/commit/384e9a8336536db1acf142568ddf13c4b0a1450a)), closes [#27](https://github.com/Mahlet-Inc/hobbits/issues/27)

# [0.24.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.23.0...v0.24.0) (2020-04-09)


### Bug Fixes

* return  value in RangeHighlight DataStream operator ([e8ec1b0](https://github.com/Mahlet-Inc/hobbits/commit/e8ec1b05edd2fbc77d6464923ccc30f878ac478c))


### Features

* add basic Kaitai Struct and Extractor plugins for [#24](https://github.com/Mahlet-Inc/hobbits/issues/24) ([6278b6b](https://github.com/Mahlet-Inc/hobbits/commit/6278b6b5ebdee795fc510fd2a95afe9a3ce71f41))
* add recent imports to File menu for convenience ([fc2fba7](https://github.com/Mahlet-Inc/hobbits/commit/fc2fba7193833126c4e68b66456fa2455355013c))

# [0.23.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.22.1...v0.23.0) (2020-03-12)


### Bug Fixes

* significantly speed-up byte raster rendering ([a434cfa](https://github.com/Mahlet-Inc/hobbits/commit/a434cfad0d9bbb66aa65e3092590703292c77d39)), closes [#19](https://github.com/Mahlet-Inc/hobbits/issues/19)


### Features

* add CP437 as optional display format in ASCII display for [#20](https://github.com/Mahlet-Inc/hobbits/issues/20) ([50c3d50](https://github.com/Mahlet-Inc/hobbits/commit/50c3d50925b83b76ed611cce06fcebf98975e9be))
* auto-locate python in PATH for Python Runner for [#1](https://github.com/Mahlet-Inc/hobbits/issues/1) ([f891407](https://github.com/Mahlet-Inc/hobbits/commit/f89140782bcdc578a602a652d95d01e1fb01d3e9))

## [0.22.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.22.0...v0.22.1) (2020-03-11)


### Bug Fixes

* widthframer no longer accumulates autocorrelation data into NaNs ([362f27b](https://github.com/Mahlet-Inc/hobbits/commit/362f27b7f1e20f9820299d71bb89f3155f134b54))

# [0.22.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.21.0...v0.22.0) (2020-03-10)


### Features

* add interleaving and frame-based modes to Take Skip operator ([bcbd804](https://github.com/Mahlet-Inc/hobbits/commit/bcbd80469ddabfab829b1d781e603a49c3f24428))
* add interrupting to HTTP importer downloads for [#8](https://github.com/Mahlet-Inc/hobbits/issues/8) ([b1d63dc](https://github.com/Mahlet-Inc/hobbits/commit/b1d63dcabf169821cc99e16cac56a4ce4c673c38))

# [0.21.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.20.4...v0.21.0) (2020-03-04)


### Bug Fixes

* **runner:** make output prefix "-o" option available ([c4412dd](https://github.com/Mahlet-Inc/hobbits/commit/c4412dd9e0b8ebf59b838e4e963bd16eeefe6426))


### Features

* add support for loading data larger than 42 MB ([75836d0](https://github.com/Mahlet-Inc/hobbits/commit/75836d04bf6930946e02f2a9da0dd629669eeb54))

## [0.20.4](https://github.com/Mahlet-Inc/hobbits/compare/v0.20.3...v0.20.4) (2020-02-26)


### Bug Fixes

* **release:** use a CentOS 7.4 builder with 7.4 repos for compatibility ([b1192e4](https://github.com/Mahlet-Inc/hobbits/commit/b1192e4ae459e09627dce88d335d44748e5d1745))

## [0.20.3](https://github.com/Mahlet-Inc/hobbits/compare/v0.20.2...v0.20.3) (2020-02-20)


### Bug Fixes

* **release:** fix formatting of windows version number in build ([2878e52](https://github.com/Mahlet-Inc/hobbits/commit/2878e52e00b2fcdbc7819a2bc066d03c936b3fca))

## [0.20.2](https://github.com/Mahlet-Inc/hobbits/compare/v0.20.1...v0.20.2) (2020-02-20)


### Bug Fixes

* **release:** adjust how the version number is determined pre build ([9138acf](https://github.com/Mahlet-Inc/hobbits/commit/9138acfefce4af9b492c3c58c5deb34b121ca9c8))
* **release:** give release builds updated semantic-release version ([cd34bc0](https://github.com/Mahlet-Inc/hobbits/commit/cd34bc0460769ff896a1057d76509599bf1d4f17))

## [0.20.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.20.0...v0.20.1) (2020-02-19)


### Bug Fixes

* **release:** correct references to built binaries when releasing ([6c53a52](https://github.com/Mahlet-Inc/hobbits/commit/6c53a5208dbac592c68fb9dd8f5e36a9b94ab658))

# [0.20.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.19.0...v0.20.0) (2020-02-19)


### Features

* automate releases and versioning ([e28f382](https://github.com/Mahlet-Inc/hobbits/commit/e28f38241fb4e3c0c885236cf087fc2879c7d82a))
