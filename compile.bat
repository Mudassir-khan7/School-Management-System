@echo off
echo ===================================================
echo   Compiling School Management System (C++17)
echo ===================================================
g++ -std=c++17 -Wall -Wextra main.cpp student.cpp attendance.cpp examination.cpp -o SchoolManagementSystem.exe
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Compilation finished successfully!
    echo Run 'run.bat' or 'SchoolManagementSystem.exe' to start.
) else (
    echo [ERROR] Compilation failed. Please check errors above.
)
pause
