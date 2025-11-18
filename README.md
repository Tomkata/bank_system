<img width="1282" height="837" alt="image" src="https://github.com/user-attachments/assets/e94e69db-ee80-402b-b1d1-b0fb997e1cfd" /># 🏦 Banking System in C

A full-featured banking management system written in C with SQLite database and modern web interface. Built as an educational project to demonstrate database integration, RESTful API design, and web development with C backend.

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Database](https://img.shields.io/badge/Database-SQLite3-green.svg)
![Web](https://img.shields.io/badge/Web-HTML%2FCSS%2FJS-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)

---


## ✨ Features

### Console Application
- ✅ Create new bank accounts
- ✅ View all accounts
- ✅ Deposit money
- ✅ Withdraw money
- ✅ Transfer between accounts
- ✅ Check balance
- ✅ Delete accounts
- ✅ Transaction history
- ✅ Data persistence with SQLite

### Web Interface
- 🌐 Real-time dashboard with statistics
- 📊 View all accounts in responsive table
- 💰 Deposit/Withdrawal operations
- 💸 Transfer funds between accounts
- 🔍 Account details and transaction history
- 📱 Mobile-responsive design
- 🎨 Modern UI with smooth animations

---

## 📸 Screenshots
<img width="1282" height="837" alt="image" src="https://github.com/user-attachments/assets/2cd14dd0-44fb-4f85-8c72-6d89d3cd1091" />
<img width="1344" height="910" alt="image" src="https://github.com/user-attachments/assets/5f321461-7a88-47d3-adfb-354ac83ccbe3" />
<img width="1135" height="828" alt="image" src="https://github.com/user-attachments/assets/6364c284-91ab-4a5f-a3ae-2bd57cdd2da0" />
<img width="1038" height="910" alt="image" src="https://github.com/user-attachments/assets/b56f7ee3-ca37-49a2-9890-3ffef00e1fbd" />




### Console Interface
```
╔════════════════════════════════════════╗
║    БАНКОВА СИСТЕМА ЗА УПРАВЛЕНИЕ      ║
╠════════════════════════════════════════╣
║  1. Създай нов акаунт                 ║
║  2. Покажи всички акаунти             ║
║  3. Депозит                           ║
║  4. Теглене                           ║
║  5. Трансфер между акаунти            ║
║  6. Провери баланс                    ║
║  7. Изтрий акаунт                     ║
║  0. Изход                             ║
╚═══════════════════════════════════════╝
```

### Web Dashboard
The web interface provides:
- **Dashboard**: Total balance, account count, average balance statistics
- **Deposit/Withdraw**: Quick money operations
- **Transfer**: Seamless account-to-account transfers
- **Account Details**: Complete transaction history

---

## 🛠️ Technologies

### Backend
- **Language**: C (C99 standard)
- **Database**: SQLite3
- **Web Server**: Mongoose (embedded HTTP server)
- **Compiler**: MinGW-w64 / MSYS2

### Frontend
- **HTML5** - Structure
- **CSS3** - Modern styling with animations
- **JavaScript** - Dynamic interactions
- **Font Awesome** - Icons

---

## 📁 Project Structure

```
bank_system/
├── src/                          # Source files
│   ├── main.c                    # Console application entry point
│   ├── server.c                  # HTTP server and REST API
│   ├── account.c                 # Account operations logic
│   ├── database.c                # SQLite database operations
│   ├── helpers.c                 # Utility functions
│   └── sqlite3.c                 # SQLite3 amalgamation
│
├── include/                      # Header files
│   ├── account.h                 # Account structures and functions
│   ├── database.h                # Database interface
│   ├── helpers.h                 # Helper function declarations
│   └── sqlite3.h                 # SQLite3 header
│
├── lib/                          # External libraries
│   ├── mongoose.c                # Mongoose web server
│   └── mongoose.h
│
├── Web/                          # Web interface (frontend)
│   ├── index.html                # Dashboard page
│   ├── deposit_withdraw.html     # Deposit/Withdraw page
│   ├── transfer.html             # Transfer page
│   ├── account_details.html      # Account details page
│   ├── style.css                 # Main stylesheet
│   ├── app.js                    # Dashboard JavaScript
│   ├── deposit_withdraw.js       # Deposit/Withdraw logic
│   ├── transfer.js               # Transfer logic
│   └── account_details.js        # Account details logic
│
├── data/                         # Database storage
│   └── bank.db                   # SQLite database file
│
├── build/                        # Compiled executables
│   ├── bank_system.exe           # Console application
│   └── bank_server.exe           # Web server application
│
├── compile_server.bat            # Build script for server
└── README.md                     # This file
```

---

## 💻 Installation

### Prerequisites
- **Windows OS** (or WSL for Linux)
- **MinGW-w64** or **MSYS2** with GCC compiler
- **Make** (optional, for build automation)

### Setup

1. **Clone the repository**
   ```bash
   git clone https://github.com/Tomkata/bank-system-c.git
   cd bank-system-c
   ```

2. **Compile the console application**
   ```bash
   gcc -I include src/account.c src/database.c src/main.c src/sqlite3.c src/helpers.c -o bank_system.exe -Wall
   ```

3. **Compile the web server**
   ```bash
   gcc -I include src/account.c src/database.c src/server.c src/sqlite3.c src/helpers.c lib/mongoose.c -o bank_server.exe -lws2_32 -Wall
   ```

   Or use the provided batch script:
   ```bash
   compile_server.bat
   ```

---

## 🚀 Usage

### Console Application

Run the console version:
```bash
./bank_system.exe
```

Follow the menu to perform banking operations.

### Web Application

1. Start the web server:
   ```bash
   ./bank_server.exe
   ```

2. Open your browser and navigate to:
   ```
   http://localhost:8000
   ```

3. Use the web interface to manage accounts, perform transactions, and view statistics.

---

## 🔌 API Endpoints

The server exposes the following REST API endpoints:

### Accounts
- `GET /api/accounts` - Get all accounts
- `POST /api/accounts` - Create new account
  ```json
  {
    "owner_name": "John Doe",
    "initial_deposit": 1000.00
  }
  ```
- `GET /api/accounts/:id` - Get account by ID
- `DELETE /api/accounts/:id` - Delete account

### Transactions
- `POST /api/deposit` - Deposit money
  ```json
  {
    "account_id": 1,
    "amount": 500.00
  }
  ```
- `POST /api/withdraw` - Withdraw money
  ```json
  {
    "account_id": 1,
    "amount": 200.00
  }
  ```
- `POST /api/transfer` - Transfer between accounts
  ```json
  {
    "from_account_id": 1,
    "to_account_id": 2,
    "amount": 300.00
  }
  ```

### Statistics
- `GET /api/stats` - Get system statistics

---



## 🗺️ Roadmap

Future improvements planned:

- [ ] User authentication and authorization
- [ ] Password-protected accounts
- [ ] Interest rate calculations
- [ ] Loan management system
- [ ] Multi-currency support
- [ ] Account statements (PDF export)
- [ ] Email notifications
- [ ] Advanced search and filters
- [ ] Transaction categories
- [ ] Budget tracking
- [ ] Dark mode for web interface
- [ ] Docker containerization
- [ ] Linux/macOS support

---

## 👨‍💻 Author

**Toma Andreev**  
Naval Academy Student

[![GitHub](https://img.shields.io/badge/GitHub-Tomkata-black?logo=github)](https://github.com/Tomkata)
[[![LinkedIn](https://bg.linkedin.com/in/toma-andreev-05a7b6399?trk=people-guest_people_search-card)
[![Instagram](https://img.shields.io/badge/Instagram-toma__andreev-purple?logo=instagram)](https://www.instagram.com/toma_andreev/)

---

## 📄 License

This project is created for **educational purposes** as part of programming coursework at the Naval Academy.

Feel free to use this code for learning and reference.

---


---

## 📧 Contact

For questions, suggestions, or collaboration:
- Email: tomaandreev12@gmail.com
- LinkedIn: https://www.linkedin.com/in/toma-andreev-05a7b6399/

---
