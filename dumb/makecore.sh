#!/bin/sh

PREFIX="/usr/local"
INSTALL=instcore
TARGET=core

if [ "$1" = "--prefix" ]; then
    PREFIX="$2"
elif [ "$1" != "" ]; then
    cat << EOF
Usage: install [--prefix dir]

EOF
    exit
fi

echo "Installing DUMB core into $PREFIX"

make $TARGET
make $INSTALL PREFIX=$PREFIX
make $INSTALL PREFIX=$PREFIX DEBUGMODE=1
