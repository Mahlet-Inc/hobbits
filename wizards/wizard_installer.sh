#!/bin/sh

export TEMPLS_DIR=$HOME/.config/QtProject/qtcreator/templates/wizards

# make the wizard directory if it doesn't exist
mkdir -p $TEMPLS_DIR

# copy new wizards to wizard directory
for n in analyzer display importexport operator; do
	export PLGDIR="$TEMPLS_DIR/hobbits-plugin-${n}"
	echo $n $PLGDIR;
	rm -rf $PLGDIR;
	mkdir -p $PLGDIR
	cp -r "${n}" $PLGDIR;
done
