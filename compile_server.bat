@echo off
chcp 65001 > nul
cls

echo Compiling HTTP Server...

gcc -I lib ^
    src/server.c ^
    lib/mongoose.c ^
    -o bank_server.exe ^
    -lws2_32 ^
    -Wall

if %ERRORLEVEL% EQU 0 (
    echo Success!
    echo.
    echo Starting server on http://localhost:8000
    echo.
    bank_server.exe
) else (
    echo Failed!
    pause
)