## [0.38.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.38.0...v0.38.1) (2020-09-12)


### Bug Fixes

* correct some spectrogram rendering issues and a crash ([790e169](https://github.com/Mahlet-Inc/hobbits/commit/790e169345455576eec828b06c27869893891359))


### Performance Improvements

* prevent excessive disk allocation by variable frame widths ([beda5de](https://github.com/Mahlet-Inc/hobbits/commit/beda5de35b3b3dc0abd85a882549e6ffd55644a3))

# [0.38.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.37.0...v0.38.0) (2020-09-08)


### Bug Fixes

* kill render threads on program exit ([ca80c21](https://github.com/Mahlet-Inc/hobbits/commit/ca80c21e08507874614292b4f0a55a315fb82bea))


### Features

* [api] expand PluginStateHelper with QComboBox handling ([8dece5d](https://github.com/Mahlet-Inc/hobbits/commit/8dece5d9a339fe3bfd677671554b4a854a92f1f4))
* add hover details and slice plots to spectrogram ([231af9f](https://github.com/Mahlet-Inc/hobbits/commit/231af9fbfaf02e87b32bf0b6c6430f831635693c))
* add UDP importer/exporter ([e06c4aa](https://github.com/Mahlet-Inc/hobbits/commit/e06c4aad15724903648a7432c0568ab5e186ef25))
* make spectrogram render on a separate thread ([3586739](https://github.com/Mahlet-Inc/hobbits/commit/3586739466eee2b244fb2dd904db5f9725f214ae))
* make viridis the default spectrogram color map ([30f5e90](https://github.com/Mahlet-Inc/hobbits/commit/30f5e90fb69769a628b78b8ef8822bffaf5dfe50))


### Performance Improvements

* [api] rehaul BitArray copying (currently only affects take skip) ([a90270f](https://github.com/Mahlet-Inc/hobbits/commit/a90270fd8f12c235b5e1854601f63bf54d64af86))

# [0.37.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.36.0...v0.37.0) (2020-08-27)


### Features

* add endianness controls to spectrogram ([d950d6d](https://github.com/Mahlet-Inc/hobbits/commit/d950d6df8def428ddbf67112c35bb8696eeebcc0))
* add time and frequency axes to spectrogram ([b4ed14d](https://github.com/Mahlet-Inc/hobbits/commit/b4ed14de0226d47778fb55e6a18af4c2200d56a4))
* include pre-compiled formats in kaitai struct plugin ([e7b21db](https://github.com/Mahlet-Inc/hobbits/commit/e7b21db12ba440dc1fcb5d9c5f9041ba0ca9a364))

# [0.36.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.35.0...v0.36.0) (2020-08-25)


### Features

* add simple spectrogram display ([88db8e7](https://github.com/Mahlet-Inc/hobbits/commit/88db8e7b320ee42c68abeddd54f45613ce1ef6ff))

# [0.35.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.34.1...v0.35.0) (2020-08-20)


### Features

* embed python into hobbits with a limited API for [#23](https://github.com/Mahlet-Inc/hobbits/issues/23) ([2f4ef03](https://github.com/Mahlet-Inc/hobbits/commit/2f4ef03fcb9f62dc96eec960e5d1c8a038cb6f96))

## [0.34.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.34.0...v0.34.1) (2020-07-17)


### Bug Fixes

* fix bugs and improve performance in Bit Error plugin ([e988e7c](https://github.com/Mahlet-Inc/hobbits/commit/e988e7c36aa4e38093ee2624939aa82e5f516293))
* improve stability of preview scrollbar a little bit ([7783a67](https://github.com/Mahlet-Inc/hobbits/commit/7783a67fe4f53a63b651185bc9a15ef9d50940e6))
* properly initialize de/interleave controls in Take Skip plugin ([50a295a](https://github.com/Mahlet-Inc/hobbits/commit/50a295a9b6093a9ced52c69be8307145f4c1af60))

# [0.34.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.33.0...v0.34.0) (2020-07-01)


### Features

* add a "delete all containers" action ([56b77f4](https://github.com/Mahlet-Inc/hobbits/commit/56b77f47d33f5b163df1b6f77ddc6ad6ca9c7202))
* add deinterleaving to the take skip plugin ([f04f6a1](https://github.com/Mahlet-Inc/hobbits/commit/f04f6a1c9874bc7466d900d57bc14e3c0fcf83c5))
* add repeats and state saving to hex string import manual mode ([6d2ae1f](https://github.com/Mahlet-Inc/hobbits/commit/6d2ae1f7a967056f0b4add58c637f63a9a0ff1ac))
* save window dock state and size between runs ([a90fe45](https://github.com/Mahlet-Inc/hobbits/commit/a90fe450efea072fb570b8d919f357524854f256))

# [0.33.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.32.1...v0.33.0) (2020-06-26)


### Features

* add windows bat file for wizard installation for [#49](https://github.com/Mahlet-Inc/hobbits/issues/49) [#50](https://github.com/Mahlet-Inc/hobbits/issues/50) ([199570f](https://github.com/Mahlet-Inc/hobbits/commit/199570f99b9adc1afa87c4e0692b1826a085a465))
* redesigned plugin action management, replacing "templates" with "batches"; resolves [#15](https://github.com/Mahlet-Inc/hobbits/issues/15) ([#51](https://github.com/Mahlet-Inc/hobbits/issues/51)) ([78da500](https://github.com/Mahlet-Inc/hobbits/commit/78da500359d2405d2a6f95fc5c7d4a7da7e2246c))

## [0.32.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.32.0...v0.32.1) (2020-06-11)


### Bug Fixes

* enable Python versions lower than 3.6 to work with kaitai plugin ([fa12a87](https://github.com/Mahlet-Inc/hobbits/commit/fa12a8786d44f4ec900af655dedb1b3305d31f9e))
* run kaitai-struct-compiler correctly in windows ([8b51480](https://github.com/Mahlet-Inc/hobbits/commit/8b514806a94ff13ff05d2c6e0c7deff86df05f64))

# [0.32.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.31.1...v0.32.0) (2020-05-27)


### Features

* add "pre-pad" and "byte-aligned" options to Header Framer ([70f765f](https://github.com/Mahlet-Inc/hobbits/commit/70f765f733bdb2b05aa09306b8fc9c8ba2eeb6a6))
* add byte-based Take Skip syntax (uppercase letters) for [#43](https://github.com/Mahlet-Inc/hobbits/issues/43) ([f7c74bf](https://github.com/Mahlet-Inc/hobbits/commit/f7c74bfeddecefd2238b3f05725f55827efaebe0))

## [0.31.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.31.0...v0.31.1) (2020-05-08)


### Bug Fixes

* preview render crash on small/empty bit containers ([d58ad89](https://github.com/Mahlet-Inc/hobbits/commit/d58ad8963ee71793cf83ded25eb9667f63590e3f))

# [0.31.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.30.2...v0.31.0) (2020-05-04)


### Features

* improve default operator output container names ([f630c52](https://github.com/Mahlet-Inc/hobbits/commit/f630c5296a5b3333813e4bcef594c80012efb6ce))

## [0.30.2](https://github.com/Mahlet-Inc/hobbits/compare/v0.30.1...v0.30.2) (2020-04-30)


### Bug Fixes

* put bitarray swap in the system temp path vs a relative path ([599a657](https://github.com/Mahlet-Inc/hobbits/commit/599a657317e02c59bd0ed709378b0d4dc85e7b5f)), closes [#37](https://github.com/Mahlet-Inc/hobbits/issues/37)

## [0.30.1](https://github.com/Mahlet-Inc/hobbits/compare/v0.30.0...v0.30.1) (2020-04-30)


### Bug Fixes

* remove kaitai struct definiteions with incompatible licenses ([d9513d6](https://github.com/Mahlet-Inc/hobbits/commit/d9513d69a7bbc7b1d8a0ba458387f4a9a9c3d960))

# [0.30.0](https://github.com/Mahlet-Inc/hobbits/compare/v0.29.0...v0.30.0) (2020-04-29)


### Features

* improve kaitai integration with new nested range highlights ([6b781a8](https://github.com/Mahlet-Inc/hobbits/commit/6b781a8725eab9229306f9bf7b53f70c06e6844e))

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
