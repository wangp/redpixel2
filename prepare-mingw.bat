@echo off

rem 
rem You must run this script once before compiling with MinGW.
rem 

echo "Preparing libnet"
copy libnet\makfiles\mingw.mak libnet\port.mak

echo "Preparing DUMB"
cd dumb
fix.bat mingw
cd ..
