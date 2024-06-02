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

rc /nologo /fo app-manifest.res app-manifest.rc

if not defined CL (
    set CL=/nologo ^
        /utf-8 /EHsc /GR /permissive- /std:c++17 /Zc:__cplusplus /Zc:externC- ^
        /W4 /wd4459 /D _CRT_SECURE_NO_WARNINGS=1 /D _STL_SECURE_NO_WARNINGS=1
)
set MICROLIBS="%~dp0..\microlibs"
cl /I %MICROLIBS% -D FMT_HEADER_ONLY -D UTF8_WINAPI /Fe"b" ^
    %CPPFILE% cppm.cpp ^
    app-manifest.res
