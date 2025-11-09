#include <stdio.h>
#include "sqlite3.h"

int main(void) {
    printf("Testing SQLite...\n");
    printf("SQLite version: %s\n", sqlite3_libversion());
    
    sqlite3 *db;
    int rc = sqlite3_open("test.db", &db);
    
    if (rc) {
        printf(" Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    } else {
        printf("Opened database successfully!\n");
    }
    
    sqlite3_close(db);
    return 0;
}