#!/usr/bin/env bash

echo "There is a long-ignored bug in CLang. https://bugs.llvm.org/show_bug.cgi?id=44594 . Have to work around."

$(
	echo ld
	i=0
	for el in $@; do
		if [ -e "$el" ]; then
			fn=$(basename $el);
			ext=${fn##*.}
			dn=$(dirname $el);
			if [[ $dn == *"/lib/gcc/"* ]]; then
				if [ "$ext" = "o" ]; then
					bn="${fn%.*}";
					newFulN="$dn/${bn}S.o";
					if [ -e "$newFulN" ]; then
						el=$newFulN;
					fi;
				fi;
			fi;
		fi;
		echo $el;
	done;
) | xargs
