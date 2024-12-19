#include <stdio.h>
#include "parser.h"
#include "database.h"
//gcc -Wall -Wextra -Wno-unused-value -Iinclude -Ivendor/hash -Ivendor/vector src/main.c src/database.c src/parser.c vendor/hash/hash.c vendor/vector/vector.c -o database.exe

int main() {
    char input[256];
    init_db();

    printf("Simple Database Shell. Type 'exit' to quit.\n");

    while (1) {
        printf("db> ");
        fgets(input, sizeof(input), stdin);
        if (strcmp(input, "exit\n") == 0) break;
        parse_command(input);
    }

    free_db();
    return 0;
}