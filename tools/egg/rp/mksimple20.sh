rm -f *.tga
../eggshell simple20.egg -o "" -bpp 16 -size 20 20 -frames 16
rm -f simple20.dat
dat -a simple20.dat -c2 -bpp 16 -t BMP *.tga
