@echo off & setlocal enableextensions
if "%1"=="" (
    echo !Usage: %~n0 CPPFILENAME [COMPILER_OPTIONS] 1>&2
    exit /b 1
)
set CPPFILE=%1
if not exist "%CPPFILE%" (
    echo !No "%CPPFILE%" file in the current directory. 1>&2
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
rc /nologo /fo app-manifest.res "%CPPDIR%\app-manifest.rc"

echo Compiling and linking...
if not defined CL (
    set CL=/nologo ^
        /utf-8 /EHsc /GR /permissive- /std:c++17 /Zc:__cplusplus /Zc:externC- ^
        /W4 /wd4459 /D _CRT_SECURE_NO_WARNINGS=1 /D _STL_SECURE_NO_WARNINGS=1
)

cl /I %MICROLIBSDIR% -D FMT_HEADER_ONLY /Fe"b" ^
    %CPPFILE% "%CPPDIR%\cppm.cpp" ^
    app-manifest.res user32.lib version.lib ^
    %COMPILER_OPTIONS%
