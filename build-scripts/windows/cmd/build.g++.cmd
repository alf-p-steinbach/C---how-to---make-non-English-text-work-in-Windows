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
set CPPDIR=%~dp1
if not exist "%CPPDIR%"\cppm.cpp (
    echo !No cppm.cpp file in the source directory; you can just copy one there.
    exit /b 1
)
set MICROLIBSDIR="%~dp0..\..\..\microlibs"

rem set COMPILER_OPTIONS to text of args %1 through n.
for /f "tokens=1,* delims= " %%a in ("%*") do set COMPILER_OPTIONS=%%b


echo Generating binary resource...
windres -i app-manifest.rc -o app-manifest.o

echo Compiling and linking...
if not defined CPATH (
    echo !Set env. variable `C_PATH` to include a directory so ^<fmt/core.h^> is found. 1>&2
    exit /b 1
)
set STDCPP=-std=c++17 -pedantic-errors -Wall -Wextra
rem MinGW g++ links user32 DLL by default also for a console subsystem build.
g++ %STDCPP% -s -I %MICROLIBSDIR% -D FMT_HEADER_ONLY ^
    %CPPFILE% cppm.cpp ^
    app-manifest.o -lversion ^
    %COMPILER_OPTIONS%
