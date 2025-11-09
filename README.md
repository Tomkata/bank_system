# Bank Management System 🏦

Simple bank account management system written in C with SQLite database.

## Features

✅ Create bank accounts
✅ Deposit money
✅ Withdraw money
✅ Transfer between accounts
✅ Transaction history
✅ SQLite database for persistence
✅ Search accounts by name
✅ Filter accounts by balance
✅ Statistics (total balance, average, etc.)

## Technologies

- **Language:** C
- **Database:** SQLite3
- **Platform:** Windows (MinGW/MSYS2)

## Project Structure
```
bank_system/
├── include/           # Header files
│   ├── account.h
│   └── database.h
├── src/               # Source files
│   ├── account.c
│   ├── database.c
│   ├── main.c
│   └── sqlite3.c
├── data/              # Database storage
│   └── bank.db
└── README.md
```

## Compilation
```bash
gcc -I include src/account.c src/database.c src/main.c src/sqlite3.c -o bank_system.exe -Wall
```

## Usage
```bash
./bank_system.exe
```

## Author

Toma - Naval Academy Student

## License

This project is for educational purposes.