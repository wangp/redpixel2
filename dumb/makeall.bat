@echo off
make
IF ERRORLEVEL 1 GOTO :fail
make install
IF ERRORLEVEL 1 GOTO :fail
make DEBUGMODE=1 install
IF NOT ERRORLEVEL 1 GOTO :success
:fail
echo -
echo An error occurred. Please see faq.txt to see if it solves your problem. If you
echo still have trouble, please contact me and I'll try to help you sort it out. See
echo readme.txt for contact details. Please be sure to give as much information as
echo possible.
:success