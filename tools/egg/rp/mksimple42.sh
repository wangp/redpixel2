rm -f *.tga
../eggshell simple42.egg -o "" -bpp 16 -size 42 42 -frames 21 -shoot 4 1 
rm -f simple42.dat
dat -a simple42.dat -c2 -bpp 16 -t BMP *.tga
