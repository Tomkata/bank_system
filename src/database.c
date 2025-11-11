#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"


sqlite3 *db = NULL;

int init_database(const char *db_path)
{
    int rc = sqlite3_open(db_path, &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно отваряне на база данни: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    printf("Успешно отворена база данни.\n");

    const char *sql_accounts = "CREATE TABLE IF NOT EXISTS accounts ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "owner_name TEXT NOT NULL,"
                               "balance REAL NOT NULL DEFAULT 0,"
                               "created_at INTEGER NOT NULL,"
                               "is_active INTEGER NOT NULL DEFAULT 1);";

    const char *sql_transactions = "CREATE TABLE IF NOT EXISTS transactions ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "account_id INTEGER NOT NULL,"
                                   "type INTEGER NOT NULL,"
                                   "amount REAL NOT NULL,"
                                   "timestamp INTEGER NOT NULL,"
                                   "description TEXT,"
                                   "FOREIGN KEY(account_id) REFERENCES accounts(id));";

    char *err_msg = NULL;

    // accounts table
    rc = sqlite3_exec(db, sql_accounts, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL грешка при създаване на таблица accounts: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    rc = sqlite3_exec(db, sql_transactions, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL грешка при създаване на таблица transactions: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Таблиците са създадени успешно.\n");
    return 0;
}

int close_database()
{
    if (db)
    {
        sqlite3_close(db);
        db = NULL;
    }
    return 0;
}

int db_create_account(const char *owner_name, double initial_balance)
{
    if (initial_balance < 0)
    {
        fprintf(stderr, "Началният баланс не може да бъде отрицателен.\n");
        return -1;
    }

    const char *sql = "INSERT INTO accounts (owner_name, balance, created_at, is_active) "
                      "VALUES (?, ?, ?, 1);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешна подготовка на заявка: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, owner_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, initial_balance);
    sqlite3_bind_int(stmt, 3, (int)time(NULL));

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    int new_id = (int)sqlite3_last_insert_rowid(db);

    if (initial_balance > 0)
    {
        db_add_transaction(new_id, DEPOSIT, initial_balance, "Initial deposit");
    }
    printf("Account created successfully! ID: %d\n", new_id);

    return new_id;
}

Account* db_find_account(int account_id)
{
    const char* sql =
        "SELECT id, owner_name, balance, created_at, is_active "
        "FROM accounts WHERE id = ? AND is_active = 1;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, account_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return NULL;
    }

    Account* acc = malloc(sizeof(Account));
    if (!acc)
    {
        sqlite3_finalize(stmt);
        return NULL;
    }

    acc->id = sqlite3_column_int(stmt, 0);
    const unsigned char* name_text = sqlite3_column_text(stmt, 1);
    if (name_text)
    {
        strncpy(acc->owner_name, (const char*)name_text, MAX_NAME - 1);
        acc->owner_name[MAX_NAME - 1] = '\0';
    }
    else
    {
        strcpy(acc->owner_name, "(Unknown)");
    }
    acc->balance = sqlite3_column_double(stmt, 2);
    acc->created_at = sqlite3_column_int(stmt, 3);
    acc->is_active = sqlite3_column_int(stmt, 4);

    sqlite3_finalize(stmt);
    return acc;   
}


int db_get_all_accounts(Account **accounts, int *count)
{
    const char *sql = "SELECT id, owner_name, balance, created_at, is_active "
                      "FROM accounts WHERE is_active = 1 ORDER BY id;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    int temp_count = 0;
    Account *temp_accounts = malloc(MAX_ACCOUNTS * sizeof(Account));

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Account *acc = &temp_accounts[temp_count];

        acc->id = sqlite3_column_int(stmt, 0);
        strncpy(acc->owner_name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME - 1);
        acc->owner_name[MAX_NAME - 1] = '\0';
        acc->balance = sqlite3_column_double(stmt, 2);
        acc->created_at = sqlite3_column_int(stmt, 3);
        acc->is_active = sqlite3_column_int(stmt, 4);

        temp_count++;
    }
    sqlite3_finalize(stmt);

    *accounts = temp_accounts;
    *count = temp_count;

    return 0;
}

int db_update_account_balance(int account_id, double new_balance)
{
    const char *sql = "UPDATE accounts SET balance = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_double(stmt, 1, new_balance);
    sqlite3_bind_int(stmt, 2, account_id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_add_transaction(int account_id, TransactionType type,
                       double amount, const char *description)
{
    const char *sql = "INSERT INTO transactions (account_id, type, amount, timestamp, description) "
                      "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, account_id);
    sqlite3_bind_int(stmt, 2, (int)type);
    sqlite3_bind_double(stmt, 3, amount);
    sqlite3_bind_int(stmt, 4, (int)time(NULL));
    sqlite3_bind_text(stmt, 5, description, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

double db_get_balance(int account_id)
{
    Account *acc = db_find_account(account_id);
    if (acc == NULL)
    {
        return -1.0;
    }
    return acc->balance;
}

int db_delete_account(int account_id)
{
    const char *sql = "UPDATE accounts SET is_active = 0 WHERE id = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, account_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        printf("Профил #%d е успешно изтрит.\n", account_id);
        return 0;
    }

    return -1;
}

// search
int db_search_accounts_by_name(const char *owner_name, Account **accounts, int *count)
{
    // query
    const char *sql = "SELECT id, owner_name, balance, created_at, is_active "
                      "FROM accounts WHERE owner_name LIKE ? AND is_active = 1 "
                      "ORDER BY owner_name;";

    // statement
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    char search_pattern[MAX_NAME + 3];
    snprintf(search_pattern, sizeof(search_pattern), "%%%s%%", owner_name);
    sqlite3_bind_text(stmt, 1, search_pattern, -1, SQLITE_STATIC);

    int temp_count = 0;

    Account *temp_accounts = malloc(MAX_ACCOUNTS * sizeof(Account));

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {

        Account *acc = &temp_accounts[temp_count];

        acc->id = sqlite3_column_int(stmt, 0);
        strncpy(acc->owner_name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME - 1);
        acc->owner_name[MAX_NAME - 1] = '\0';
        acc->balance = sqlite3_column_double(stmt, 2);
        acc->created_at = sqlite3_column_int(stmt, 3);
        acc->is_active = sqlite3_column_int(stmt, 4);

        temp_count++;
    }
    sqlite3_finalize(stmt);

    *accounts = temp_accounts;
    *count = temp_count;

    return 0;
}

int db_filter_accounts_by_balance(double min_balance, Account **accounts, int *count)
{
    const char *sql = "SELECT id, owner_name, balance, created_at, is_active "
                      "FROM accounts WHERE balance > ? AND is_active = 1 "
                      "ORDER BY balance DESC;";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_double(stmt, 1, min_balance);

    int temp_count = 0;

    Account *temp_accounts = malloc(MAX_ACCOUNTS * sizeof(Account));

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {

        Account *acc = &temp_accounts[temp_count];

        acc->id = sqlite3_column_int(stmt, 0);
        strncpy(acc->owner_name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME - 1);
        acc->owner_name[MAX_NAME - 1] = '\0';
        acc->balance = sqlite3_column_double(stmt, 2);
        acc->created_at = sqlite3_column_int(stmt, 3);
        acc->is_active = sqlite3_column_int(stmt, 4);

        temp_count++;
    }
    sqlite3_finalize(stmt);

    *accounts = temp_accounts;
    *count = temp_count;

    return 0;
}

// stats

// За преизползване
static double db_get_double_query(const char* sql)
{

    sqlite3_stmt *stmt;
    double result = 0.0;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return 0.0;
    }   

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return result;
}

double db_get_total_balance(void)
{
    return db_get_double_query("SELECT SUM(balance) FROM accounts WHERE is_active = 1;");
}

double db_get_average_balance(void)
{
    return db_get_double_query("SELECT AVG(balance) FROM accounts WHERE is_active = 1;");
}


/*
Това е статична функция за преизползване, както отгоре. Ще я преизползвам за намиране на акаунт с най-много и най-малко пари.

Ако решa да сменя структурата на таблицата, редактирам само на едно място

Лесно може да се добави и трета функция (например среден баланс) без копиране

!!!!!!!!
Този подход с snprintf() и %s отваря дупка за SQL injection, ако extreme идва от външен източник

Това съм го направил с идеята, че е проект за университета, а не реално enterprise app
*/
static Account* db_get_account_by_balance_extreme(const char *extreme)
{
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT id, owner_name, balance, created_at, is_active "
        "FROM accounts "
        "WHERE is_active = 1 "
        "AND balance = (SELECT %s(balance) FROM accounts WHERE is_active = 1)",
        extreme);

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Неуспешно подготвяне на изявление: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    Account *acc = malloc(sizeof(Account));
    if (!acc) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    acc->id = sqlite3_column_int(stmt, 0);
    const unsigned char *name_text = sqlite3_column_text(stmt, 1);
    if (name_text) {
        strncpy(acc->owner_name, (const char *)name_text, MAX_NAME - 1);
        acc->owner_name[MAX_NAME - 1] = '\0';
    } else {
        strcpy(acc->owner_name, "(Unknown)");
    }

    acc->balance   = sqlite3_column_double(stmt, 2);
    acc->created_at = sqlite3_column_int(stmt, 3);
    acc->is_active = sqlite3_column_int(stmt, 4);

    sqlite3_finalize(stmt);
    return acc;
}



Account* db_get_richest_account(void)
{

    return db_get_account_by_balance_extreme("MAX");
} 

Account* db_get_poorest_account(void)
{

    return db_get_account_by_balance_extreme("MIN");
} 



// transaction statistics
int db_get_transaction_count(int account_id, TransactionType type)
{
    if (is_valid_account_id(account_id))
    {
        return 0;
    }
    
    const char* sql = "SELECT COUNT(*) "
        "FROM transactions  "
        "WHERE account_id = ? AND type = ?";

        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
        if (rc != SQLITE_OK)
        {
        fprintf(stderr, "Грешка при подготовка: %s\n", sqlite3_errmsg(db));
                return 0;       
        }

        sqlite3_bind_int(stmt,1,account_id);
        sqlite3_bind_int(stmt,2,(int)type);
        
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt,0);
    }
    
    sqlite3_finalize(stmt);
    return count;
    
}

