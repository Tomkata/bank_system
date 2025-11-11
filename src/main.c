#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"
#include "database.h"
#include "helpers.h"

#define DATABASE_FILE "data/bank.db"

void print_menu(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║    БАНКОВА СИСТЕМА ЗА УПРАВЛЕНИЕ      ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  1. Създай нов акаунт                 ║\n");
    printf("║  2. Покажи всички акаунти             ║\n");
    printf("║  3. Депозит                           ║\n");
    printf("║  4. Теглене                           ║\n");
    printf("║  5. Трансфер между акаунти            ║\n");
    printf("║  6. Провери баланс                    ║\n");
    printf("║  7. Изтрий акаунт                     ║\n");
    printf("║  0. Изход                             ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Избери опция: ");
}





int main(void)
{
    int choice;
    char name[MAX_NAME];
    double amount;
    int id, from_id, to_id;

        system("chcp 65001 > nul");
        
    printf("\n════════════════════════════════════════\n");
    printf("  Банкова система за управление (SQLite)\n");
    printf("  Инициализация на база данни...\n");
    printf("════════════════════════════════════════\n\n");

   
    if (init_database(DATABASE_FILE) != 0) {
        printf("Неуспешна инициализация на базата данни!\n");
        return 1;
    }

    while(1)
    {
        print_menu();
        
        if (scanf("%d", &choice) != 1) 
        {
            printf("Невалиден вход!\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch(choice)
        {
            case 1:  
                printf("\n═══ СЪЗДАВАНЕ НА НОВ АКАУНТ ═══\n");
                printf("Име на собственика: ");
                
                scanf(" %[^\n]", name);  
                clear_input_buffer();
                
                if(strlen(name) == 0) 
                {
                    printf("Името не може да бъде празно!\n");
                    break;
                }
                
                printf("Начален депозит (лв): ");
                if (scanf("%lf", &amount) != 1)
                 {
                    printf("Невалидна сума!\n");
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer();
                
                int new_id = create_account(name, amount);
                if(new_id > 0) {
                    printf("\nУспех! ID на новия акаунт: %d\n", new_id);
                }
                break;
            
            case 2: 
                list_all_accounts();
                break;
            
            case 3:  
                printf("\n═══ ДЕПОЗИТ ═══\n");
                printf("ID на акаунт: ");
                scanf("%d", &id);
                printf("Сума (лв): ");
                scanf("%lf", &amount);
                clear_input_buffer();
                
                deposit(id, amount);
                break;
            
            case 4: 
                printf("\n═══ ТЕГЛЕНЕ ═══\n");
                printf("ID на акаунт: ");
                scanf("%d", &id);
                printf("Сума (лв): ");
                scanf("%lf", &amount);
                clear_input_buffer();
                
                withdraw(id, amount);
                break;
            
            case 5:  
                printf("\n═══ ТРАНСФЕР ═══\n");
                printf("От акаунт (ID): ");
                scanf("%d", &from_id);
                printf("Към акаунт (ID): ");
                scanf("%d", &to_id);
                printf("Сума (лв): ");
                scanf("%lf", &amount);
                clear_input_buffer();
                
                transfer(from_id, to_id, amount);
                break;
            
            case 6:  
                printf("\n═══ ПРОВЕРКА НА БАЛАНС ═══\n");
                printf("ID на акаунт: ");
                scanf("%d", &id);
                clear_input_buffer();
                
                double balance = get_balance(id);
                if (balance >= 0) {
                    printf("Баланс на акаунт #%d: %.2f лв\n", id, balance);
                } else {
                    printf("Акаунтът не е намерен.\n");
                }
                break;
            
            case 7:  
                printf("\n═══ ИЗТРИВАНЕ НА АКАУНТ ═══\n");
                printf("ID на акаунт: ");
                scanf("%d", &id);
                clear_input_buffer();
                
                printf("Сигурен ли си? (1=Да, 0=Не): ");
                int confirm;
                scanf("%d", &confirm);
                clear_input_buffer();
                
                if (confirm == 1) {
                    delete_account(id);
                } else {
                    printf("Операцията е отменена.\n");
                }
                break;
            

            case 0:  
                printf("\n════════════════════════════════════════\n");
                printf("  Затваряне на база данни...\n");
                close_database();
                printf("  Довиждане!\n");
                printf("════════════════════════════════════════\n\n");
                return 0;
            
            default:
                printf("Невалидна опция! Избери 0-8.\n");
                break;
        }
        
        printf("\nНатисни Enter за да продължиш...");
        getchar();
    }
    
    return 0;
}