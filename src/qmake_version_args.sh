#!/bin/sh

export HOBBITS_VERSION=`git rev-parse HEAD`
if [ -e ../.version ]
then
  export HOBBITS_VERSION=`cat ../.version`
fi

echo DEFINES+=\"HOBBITS_GUI_VERSION=\\\"\\\\\\\"`echo $HOBBITS_VERSION`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_CORE_LIB_VERSION=\\\"\\\\\\\"`echo $HOBBITS_VERSION`\\\\\\\"\\\"\" DEFINES+=\"HOBBITS_RUNNER_VERSION=\\\"\\\\\\\"`echo $HOBBITS_VERSION`\\\\\\\"\\\"\"
