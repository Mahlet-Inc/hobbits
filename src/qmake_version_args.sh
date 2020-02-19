#!/bin/sh

echo DEFINES+=\"HOBBITS_GUI_VERSION=\\\"\\\\\\\"`node -pe "require('../package.json').version"`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_CORE_LIB_VERSION=\\\"\\\\\\\"`node -pe "require('../package.json').version"`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_RUNNER_VERSION=\\\"\\\\\\\"`node -pe "require('../package.json').version"`\\\\\\\"\\\"\"