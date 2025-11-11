#include "helpers.h"
#include <string.h>
#include <ctype.h>
#include <time.h>

void* safe_malloc(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)   
    {
        fprintf(stderr,"CRITICAL: Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* safe_calloc(size_t count, size_t size)
{
    void* ptr = calloc(count,size);
    if (!ptr)
    {
        fprintf(stderr,"CRITICAL: Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;

}

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' &&  c!=EOF)
    {
    }
}


int is_valid_name(const char* name)
{
    if (!name || strlen(name) == 0)
    {
        return 0;
    }
    
    int has_content = 0;
    for (size_t i = 0; i < strlen(name); i++)
     {
        if (!isspace(name[i])) 
        {
            has_content = 1;
            break;
        }
    }

        return has_content;

}

int is_valid_amount(double amount)
 {
    return (amount > 0.0 && amount <= 1000000000.0); 
}

int is_valid_account_id(int id)
{
    return (id > 0);
}

void pause_for_enter(void) 
{
    printf("\nPress Enter to continue...");
    getchar();
}

long long get_current_timestamp(void) 
{
    return (long long)time(NULL);
}

void format_currency(double amount, char* buffer, size_t size)
 {
    snprintf(buffer, size, "%.2f lv", amount);
}


