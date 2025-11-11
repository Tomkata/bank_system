#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>

//safe free - освобождава памет и сетва указателя на NULL
#define SAFE_FREE(ptr) do { \
    if (ptr) { \
        free(ptr); \
        ptr = NULL; \
    } \
} while(0)

void* safe_malloc(size_t size);
void* safe_calloc(size_t count, size_t size);

//за използване след scanf
void clear_input_buffer(void);


//validations

int is_valid_name(const char* name);

int is_valid_amount(double amount);

int is_valid_account_id(int id);

void pause_for_enter(void);

long long get_current_timestamp(void);

void format_currency(double amount, char* buffer, size_t size);

#endif