rm -f *.tga
../eggshell smoke.egg -o "" -bpp 16 -size 24 24 -frames 16
rm -f smoke.dat
dat -a smoke.dat -c2 -bpp 16 -t BMP *.tga
