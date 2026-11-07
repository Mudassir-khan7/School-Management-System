@echo off
title School Management System
color 0A

echo.
echo  ======================================================
echo       SCHOOL MANAGEMENT SYSTEM - BUILD AND RUN
echo  ======================================================
echo.

:: Step 1: Precompile student.h (only if not already done or header changed)
IF NOT EXIST student.h.gch (
    echo  [*] First run: Pre-compiling headers for faster future builds...
    g++ -std=c++17 -O0 -pipe -x c++-header student.h -o student.h.gch
    IF %ERRORLEVEL% NEQ 0 (
        echo  [ERROR] Header pre-compilation failed!
        pause
        exit /b 1
    )
    echo  [OK] Headers pre-compiled successfully!
    echo.
)

:: Step 2: Compile source files using the precompiled header
echo  [*] Compiling source files, please wait...
echo.

g++ -std=c++17 -O0 -pipe -o SchoolManagementSystem main.cpp student.cpp attendance.cpp examination.cpp

IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo  [ERROR] Compilation failed! Please check your code.
    echo.
    pause
    exit /b 1
)

echo  [SUCCESS] Compilation successful!
echo.
echo  [*] Starting School Management System...
echo  ======================================================
echo.

SchoolManagementSystem.exe

echo.
echo  ======================================================
echo  [*] Program exited. Press any key to close...
pause > nul
