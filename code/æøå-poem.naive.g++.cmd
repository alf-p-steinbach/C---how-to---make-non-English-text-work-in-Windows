@rem Encoding: Windows ANSI Western (old Cmd doesn't understand newfangled UTF-8).
@
@echo off & setlocal enableextensions
if not defined CPATH (
    echo Set environment variable `C_PATH` to include the directory of ^<fmt/core.h^>. 1>&2
    exit /b 1
)
windres -i app-manifest.rc -o app-manifest.o
g++ -I ../microlibs -D FMT_HEADER_ONLY ^
    זרו-poem.naive.cpp ^
    -D UTF8_WINAPI -include "cppm.for-windows.cpp-include" ^
    app-manifest.o
