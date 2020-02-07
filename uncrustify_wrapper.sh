#!/bin/bash

uncrustify -c uncrustify.cfg -l CPP --no-backup `find -regex './src/.*\.\(cpp\|h\)$'`