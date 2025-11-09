#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <time.h>

#define MAX_NAME 50
#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 1000

typedef enum 
{
    DEPOSIT = 0, 
    WITHDRAW = 1,
    TRANSFER = 2
} TransactionType;

typedef struct 
{
    int id;
    char owner_name[MAX_NAME];
    double balance;
    time_t created_at;
    int is_active;
} Account;

typedef struct 
{
    int id;
    int account_id;
    TransactionType type;
    double amount;
    time_t timestamp;
    char description[100];
} Transaction;


int create_account(const char* owner_name, double initial_deposit);
Account* find_account(int account_id);
int deposit(int account_id, double amount);
int withdraw(int account_id, double amount);
int transfer(int from_account_id, int to_account_id, double amount);
int delete_account(int account_id);
void list_all_accounts(void);
double get_balance(int account_id);  
int save_accounts(const char* filename);
int load_accounts(const char* filename);

#endif