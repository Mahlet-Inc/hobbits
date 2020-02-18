#!/bin/sh

pushd . > /dev/null
cd `dirname "${BASH_SOURCE[0]}"` && \
node -pe "require('./package.json').version"
popd > /dev/null