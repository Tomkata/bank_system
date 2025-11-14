#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include "account.h"

extern sqlite3 *db;

int init_database(const char* db_path);
int close_database();

//db
int db_create_account(const char* owner_name, double initial_balance);
Account* db_find_account(int account_id);
int db_get_all_accounts(Account** accounts, int* count);
int db_update_account_balance(int account_id, double new_balance);
int db_delete_account(int account_id);

int db_add_transaction(int account_id, TransactionType type, 
                       double amount, const char* description);

int db_get_transactions(int account_id, Transaction** transactions, int count);

double db_get_balance(int account_id);
int db_get_next_account_id(void);

//search
int db_search_accounts_by_name(const char* owner_name, Account** accounts, int* count);
int db_filter_accounts_by_balance(double min_balance, Account** accounts, int* count);

//stats
double db_get_total_balance();
double db_get_average_balance();
Account* db_get_richest_account(void);
Account* db_get_poorest_account(void);
int db_get_account_count(void);        

// transaction statistics
int db_get_transaction_count(int account_id, TransactionType type);
double db_get_total_deposits(int account_id);
double db_get_total_withdrawals(int account_id);

#endif
