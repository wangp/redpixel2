#! /bin/sh
#
#  Sets up the (Allegro) package for building under Unix, converting text
#  files from CR/LF to LF format.

#  Viciously stolen from Allegro!


echo "Converting files to Unix format..."

find . -type f "(" \
   -name "*.c" -o -name "*.cfg" -o -name "*.cpp" -o -name "*.dep" -o \
   -name "*.h" -o -name "*.hin" -o -name "*.in" -o -name "*.inc" -o \
   -name "*.m4" -o -name "*.mft" -o -name "*.s" -o -name "*.sh" -o \
   -name "*.pl" -o -name "*.txt" -o -name "*._tx" -o \
   -name "Makefile.*" -o -name "README" \
   ")" \
   -exec sh -c "echo {};
		mv {} _tmpfile;
		tr -d \\\r < _tmpfile > {};
		touch -r _tmpfile {}; 
		rm _tmpfile" \;

#chmod +x *.sh

echo "Done!"


