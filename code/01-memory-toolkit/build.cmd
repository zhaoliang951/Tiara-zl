@echo off
rem ============================================================
rem code/01-memory-toolkit/build.cmd  (ASCII-only version)
rem One-command build + run (Plan 5.2)
rem
rem LESSON LOG:
rem   2026-08-31 v2: UTF-8 Chinese comments broke double-click
rem   execution. cmd.exe parses .cmd files with the OEM code
rem   page (GBK on zh-CN systems), NOT UTF-8 -- multibyte chars
rem   corrupted command parsing (garbled errors, 'cl' not found,
rem   'xit' instead of 'exit'). Fix: batch files must be pure
rem   ASCII (or ANSI-encoded). Note this differs from .cpp:
rem   C++ source can opt into UTF-8 via the /utf-8 compiler flag,
rem   but batch files have no such flag.
rem ============================================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -no_logo
cd /d "%~dp0"
chcp 65001 >nul

cl /W4 /Od /EHsc /utf-8 main.cpp
if errorlevel 1 (
    echo.
    echo BUILD FAILED - screenshot this window and send it to the mentor
    pause
    exit /b 1
)

echo BUILD OK
main.exe
echo.
echo ---- press any key to close ----
pause >nul
