#!/bin/sh

export TEMPLS_DIR=$HOME/.cookiecutters

# make the cookiecutter directory if it doesn't exist
mkdir -p $TEMPLS_DIR

# copy new cookiecutters to cookiecutter directory
for n in analyzer display importexport operator; do
	export PLGDIR="$TEMPLS_DIR/hobbits-plugin-${n}"
	echo $n $PLGDIR;
	rm -rf $PLGDIR;
	mkdir -p $PLGDIR
	cp -r "${n}" $PLGDIR;
done
