@echo off
title School Management System
color 0A

echo.
echo  ======================================================
echo       SCHOOL MANAGEMENT SYSTEM - BUILD AND RUN
echo  ======================================================
echo.
echo  [*] Compiling source files, please wait...
echo.

g++ -std=c++17 -o SchoolManagementSystem main.cpp student.cpp attendance.cpp examination.cpp

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
