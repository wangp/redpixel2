#!/bin/sh

FILES="ChangeLog LICENCE.txt README.txt bjhash.inc example.c store.c store.h"
VER=0.4
VERDOS=04

mkdir -p releasetmp/store-${VER}
cp -p $FILES releasetmp/store-${VER}
cd releasetmp
tar zcf store-${VER}.tar.gz store-${VER}
mv store-${VER} store
zip -9r store${VERDOS}.zip store
