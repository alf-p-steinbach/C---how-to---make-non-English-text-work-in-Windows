@echo off & setlocal enableextensions
chcp 65001 > nul
if "%1"=="" (
    echo !Usage: %~n0 CPPFILENAME 1>&2
    exit /b 1
)
set CPPFILE="%1"
if not exist %CPPFILE% (
    echo !No %CPPFILE% file in the current directory. 1>&2
    exit /b 1
)
if not defined CPATH (
    echo !Set env. variable `C_PATH` to include a directory so ^<fmt/core.h^> is found. 1>&2
    exit /b 1
)

windres -i app-manifest.rc -o app-manifest.o

set STDCPP=-std=c++17 -pedantic-errors -Wall -Wextra
set MICROLIBS="%~dp0..\microlibs"
g++ %STDCPP% -s -I %MICROLIBS% -D FMT_HEADER_ONLY -D UTF8_WINAPI ^
    %CPPFILE% cppm.cpp ^
    app-manifest.o
