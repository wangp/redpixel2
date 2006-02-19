@echo off

rem 
rem You must run this script once before compiling with MinGW.
rem 

echo Preparing libnet...
copy libnet\makfiles\mingw.mak libnet\port.mak
echo.

echo Preparing DUMB...
cd dumb
call fix.bat mingw
cd ..
echo.

echo Done.  Use Mingw make to compile.
echo.
