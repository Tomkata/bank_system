@echo off
chcp 65001 > nul
cls

echo Compiling HTTP Server with Database...

gcc -I lib -I include ^
    src/server.c ^
    src/database.c ^
    src/account.c ^
    src/helpers.c ^
    src/sqlite3.c ^
    lib/mongoose.c ^
    -o bank_server.exe ^
    -lws2_32 ^
    -Wall ^
    -Wno-unused-but-set-variable

if %ERRORLEVEL% EQU 0 (
    echo Success!
    echo.
    bank_server.exe
) else (
    echo Failed!
    pause
)