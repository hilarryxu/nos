@echo off
cd /d %~dp0

REM 编译需要如下工具：
REM   i586-elf-gcc 交叉编译链
REM   fasm
REM   ninja
REM   python (如果已经生成好 build.ninja，可以不需要)

REM 运行需要：
REM   bochs

set path=%path%;.\Crosstools\bin

python configure.py

REM ninja -t clean
ninja all

pause
