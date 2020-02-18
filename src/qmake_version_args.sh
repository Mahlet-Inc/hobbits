#!/bin/sh

echo DEFINES+=\"HOBBITS_GUI_VERSION=\\\"\\\\\\\"`../print_version.sh`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_CORE_LIB_VERSION=\\\"\\\\\\\"`../print_version.sh`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_RUNNER_VERSION=\\\"\\\\\\\"`../print_version.sh`\\\\\\\"\\\"\"