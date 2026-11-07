@echo off
if not exist SchoolManagementSystem.exe (
    echo [INFO] Executable not found. Compiling first...
    call compile.bat
)
SchoolManagementSystem.exe
