@echo off
cd /d %~dp0

REM ������Ҫ���¹��ߣ�
REM   i586-elf-gcc ���������
REM   fasm
REM   ninja
REM   python (����Ѿ����ɺ� build.ninja�����Բ���Ҫ)

REM ������Ҫ��
REM   bochs

REM set path=%path%;.\scripts

python configure.py

ninja -t clean
ninja all

pause
