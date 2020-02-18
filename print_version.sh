#!/bin/sh

cd `dirname ${BASH_SOURCE[0]}`
node -pe "require('./package.json').version"