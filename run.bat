@echo off
g++ -o main.exe src/main.cpp
IF EXIST main.exe main.exe
py compression/myqoi.py images/result.qoi
