#include "account.h"
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int create_account(const char* owner_name, double initial_balance)
 {
    if (initial_balance < 0) {
        printf("Началният депозит не може да бъде отрицателен!\n");
        return -1;
    }
    
    if (owner_name == NULL || strlen(owner_name) == 0) {
        printf("Името не може да бъде празно!\n");
        return -1;
    }
    
    return db_create_account(owner_name, initial_balance);
}


Account* find_account(int account_id) 
{
    return db_find_account(account_id);
}


int deposit(int account_id, double amount) 
{
    if (amount <= 0) {
        printf("Сумата за депозит трябва да бъде положителна!\n");
        return -1;
    }
    
    Account* acc = db_find_account(account_id);
    if (acc == NULL) 
    {
        printf("Акаунтът с ID %d не съществува!\n", account_id);
        return -1;
    }
    
    
    double new_balance = acc->balance + amount;
    if (db_update_account_balance(account_id, new_balance) != 0) 
    {
        printf("Грешка при обновяване на баланса!\n");
        return -1;
    }
    
    
    db_add_transaction(account_id, DEPOSIT, amount, "Deposit");
    
    printf("Успешен депозит от %.2f лв. Нов баланс: %.2f лв.\n", 
           amount, new_balance);
    
    return 0;
}


int withdraw(int account_id, double amount) 
{
    if (amount <= 0) {
        printf("Сумата за теглене трябва да бъде положителна!\n");
        return -1;
    }
    
    Account* acc = db_find_account(account_id);
    if (acc == NULL) 
    {
        printf("Акаунтът с ID %d не съществува!\n", account_id);
        return -1;
    }
    
    if (acc->balance < amount)
    {
        printf("Недостатъчен баланс! Наличен: %.2f лв.\n", acc->balance);
        return -1;
    }
    
   
    double new_balance = acc->balance - amount;
    if (db_update_account_balance(account_id, new_balance) != 0) 
    {
        printf("Грешка при обновяване на баланса!\n");
        return -1;
    }
    
   
    db_add_transaction(account_id, WITHDRAW, amount, "Withdraw");
    
    printf("Успешно изтеглени %.2f лв. Нов баланс: %.2f лв.\n", 
           amount, new_balance);
    
    return 0;
}


int transfer(int from_id, int to_id, double amount)
 {
    if (amount <= 0) {
        printf("Сумата за трансфер трябва да бъде положителна!\n");
        return -1;
    }
    
    if (from_id == to_id) {
        printf("Не може да се направи трансфер към същия акаунт!\n");
        return -1;
    }
    
    Account* from = db_find_account(from_id);
    Account* to = db_find_account(to_id);
    
    if (from == NULL) 
    {
        printf("Акаунтът с ID %d не съществува!\n", from_id);
        return -1;
    }
    
    if (to == NULL) 
    {
        printf("Акаунтът с ID %d не съществува!\n", to_id);
        return -1;
    }
    
    if (from->balance < amount)
     {
        printf("Недостатъчен баланс за трансфер!\n");
        return -1;
    }
    
   
    double new_from_balance = from->balance - amount;
    double new_to_balance = to->balance + amount;
    
    db_update_account_balance(from_id, new_from_balance);
    db_update_account_balance(to_id, new_to_balance);
    
    
    char desc[100];
    snprintf(desc, 100, "Transfer to account #%d", to_id);
    db_add_transaction(from_id, TRANSFER, amount, desc);
    
    snprintf(desc, 100, "Transfer from account #%d", from_id);
    db_add_transaction(to_id, TRANSFER, amount, desc);
    
    printf("Успешен трансфер от %.2f лв. от акаунт %d към акаунт %d.\n", 
           amount, from_id, to_id);
    
    return 0;
}


int delete_account(int account_id)
 {
    Account* acc = db_find_account(account_id);
    if (acc == NULL)
     {
        printf("Акаунтът с ID %d не съществува!\n", account_id);
        return -1;
    }
    
    return db_delete_account(account_id);
}


void list_all_accounts(void)
 {
    Account* accounts = NULL;
    int count = 0;
    
    if (db_get_all_accounts(&accounts, &count) != 0) 
    {
        printf("Грешка при извличане на акаунти!\n");
        return;
    }
    
    if (count == 0) {
        printf("\nНяма налични акаунти.\n\n");
        return;
    }
    
   
    for (int i = 0; i < count; i++) 
    {
        printf("║ %3d │ %-20s │ %10.2f лв │ ✅      ║\n",
               accounts[i].id, 
               accounts[i].owner_name, 
               accounts[i].balance);
    }
    
    printf("--------------------------------------------\n");
    printf("Общо активни акаунти: %d\n\n", count);
    
    free(accounts);
}


double get_balance(int account_id)
{
    return db_get_balance(account_id);
}


