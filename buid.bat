@echo off

echo delete frames\*.txt
del asset\frames\*.txt

echo gcc compile
gcc spinning_donut.c -o donut.exe -lm

echo execute donut.exe
donut.exe

echo execute python make_gif.py
python make_gif.py

pause