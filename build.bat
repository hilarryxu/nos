@echo off
cd /d %~dp0

REM ������Ҫ���¹��ߣ�
REM   i586-elf-gcc ���������
REM   fasm
REM   ninja
REM   python (����Ѿ����ɺ� build.ninja�����Բ���Ҫ)

REM ������Ҫ��
REM   bochs

set path=%path%;.\Crosstools\bin

python configure.py

REM ninja -t clean
ninja all

pause
