#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "database.h"

void parse_command(char *input) {
    char *command = strtok(input, " \n");
    if (!command) return;

    if (strcmp(command, "CREATE") == 0) {
        handle_create_table();
    } 
    else if (strcmp(command, "DROP") == 0) {
        handle_drop_table();
    } 
    else if (strcmp(command, "ADD") == 0) {
        handle_add_column();
    } 
    else if (strcmp(command, "INSERT") == 0) {
        handle_insert_into();
    } 
    else if (strcmp(command, "LIST") == 0) {
        handle_list();
    } 
    else if (strcmp(command, "SELECT") == 0) {
        handle_select();
    }
    else if (strcmp(command, "UPDATE") == 0) {
        handle_update();
    } 
    else if (strcmp(command, "DELETE") == 0) {
        handle_delete();
    }
    else {
        printf("Error: Unknown command '%s'.\n", command);
    }
}

// CREATE TABLE
void handle_create_table() {
    char *next = strtok(NULL, " ");
    if (next && strcmp(next, "TABLE") == 0) {
        char *table_name = strtok(NULL, " \n");
        if (table_name) {
            create_table(table_name);
            printf("Table '%s' created.\n", table_name);
        } else {
            printf("Error: Missing table name.\n");
        }
    }
}

// DROP TABLE
void handle_drop_table() {
    char *next = strtok(NULL, " ");
    if (next && strcmp(next, "TABLE") == 0) {
        char *table_name = strtok(NULL, " \n");
        Table *table = find_table(table_name);
        if (table) {
            drop_table(table);
            printf("Table '%s' dropped.\n", table_name);
        } else {
            printf("Error: Table '%s' not found.\n", table_name);
        }
    }
}

// ADD COLUMN
void handle_add_column() {
    char *next = strtok(NULL, " ");
    if (next && strcmp(next, "COLUMN") == 0) {
        char *table_name = strtok(NULL, " ");
        char *column_name = strtok(NULL, " \n");
        Table *table = find_table(table_name);
        if (table && column_name) {
            add_column(table, column_name);
            printf("Column '%s' added to table '%s'.\n", column_name, table_name);
        } else {
            printf("Error: Invalid table or column name.\n");
        }
    }
}

// INSERT INTO
void handle_insert_into() {
    char *next = strtok(NULL, " ");
    if (next && strcmp(next, "INTO") == 0) {
        char *table_name = strtok(NULL, " ");
        Table *table = find_table(table_name);
        if (table) {
            char *columns[10], *values[10];
            int i = 0;
            char *pair = strtok(NULL, " \n");
            
            while (pair) {
                char *equals_sign = strchr(pair, '='); // find "="
                if (!equals_sign) {
                    printf("Error: Invalid column-value pair '%s'\n", pair);
                    return;
                }

                *equals_sign = '\0'; // split into column and value
                char *column = pair;
                char *value = equals_sign + 1;


                value = clean_quotes(value);
                columns[i] = strdup(trim(column));
                values[i] = strdup(trim(value));
                i++;

                pair = strtok(NULL, " \n");
            }

            if (i > 0) {
                insert_into(table, columns, values, i);
                printf("Row inserted into table '%s'.\n", table_name);

                for (int j = 0; j < i; j++) {
                    free(columns[j]);
                    free(values[j]);
                }
            } else {
                printf("Error: Missing column-value pairs.\n");
            }
        } else {
            printf("Error: Table '%s' not found.\n", table_name);
        }
    }
}


//LIST
void handle_list() {
    char *next = strtok(NULL, " \n");

    if (next && strcmp(next, "TABLES") == 0) {
        list_tables();
    } 
    else if (next && strcmp(next, "COLUMNS") == 0) {
        char *table_name = strtok(NULL, " \n");
        if (!table_name) {
            printf("Error: Missing table name.\n");
            return;
        }
        Table *table = find_table(table_name);
        if (table) {
            list_columns(table);
        } else {
            printf("Error: Table '%s' not found.\n", table_name);
        }
    } 
    else if (next && strcmp(next, "ROWS") == 0) {
        char *table_name = strtok(NULL, " \n");
        if (!table_name) {
            printf("Error: Missing table name.\n");
            return;
        }

        Table *table = find_table(table_name);
        if (table) {
            list_rows(table);
        } else {
            printf("Error: Table '%s' not found.\n", table_name);
        }
    } 
    else {
        printf("Error: Invalid LIST command.\n");
    }
}

// SELECT
void handle_select() {
    char *columns[10]; 
    int num_columns = 0;

    // obtain all columns till where
    char *token = strtok(NULL, " ,\n");
    while (token && strcmp(token, "WHERE") != 0) {
        columns[num_columns++] = token;
        token = strtok(NULL, " ,\n");
    }

    if (!token || strcmp(token, "WHERE") != 0) {
        printf("Error: Missing WHERE clause in SELECT command.\n");
        return;
    }

    char *table_name = strtok(NULL, " ");
    if (!table_name) {
        printf("Error: Missing table name in SELECT command.\n");
        return;
    }

    // (column=value)
    char *pair = strtok(NULL, "\n");
    if (!pair) {
        printf("Error: Missing condition in WHERE clause.\n");
        return;
    }

    char *equals = strchr(pair, '=');
    if (!equals) {
        printf("Error: Invalid WHERE clause syntax.\n");
        return;
    }

    *equals = '\0'; 
    char *column_name = trim(pair);
    char *value = clean_quotes(trim(equals + 1));

    Table *table = find_table(table_name);
    if (!table) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    // get the index of the row that meets the condition
    int row_index = where_equals(table_name, column_name, value);
    if (row_index == -1) {
        printf("No rows found where %s='%s'.\n", column_name, value);
        return;
    }

    hash_t *result = select_from(table, columns, num_columns, row_index);
    if (result) {
        printf("Debug: SELECT Result:\n");
        hash_each(result, {
            printf("%s: %s\n", key, (char *)val);
        });
        hash_free(result);
    } else {
        printf("No results found.\n");
    }
}

void handle_update() {
    char *table_name = strtok(NULL, " ");
    if (!table_name) {
        printf("Error: Missing table name.\n");
        return;
    }

    Table *table = find_table(table_name);
    if (!table) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    // check set 
    if (strcmp(strtok(NULL, " "), "SET") != 0) {
        printf("Error: Missing SET keyword.\n");
        return;
    }

    // obtain column and new value
    char *pair = strtok(NULL, " ");
    if (!pair || !strchr(pair, '=')) {
        printf("Error: Invalid column-value syntax.\n");
        return;
    }

    char *equals = strchr(pair, '=');
    *equals = '\0';
    char *column_name = trim(pair);
    char *new_value = clean_quotes(trim(equals + 1)); 

    // check where
    if (strcmp(strtok(NULL, " "), "WHERE") != 0) {
        printf("Error: Missing WHERE clause.\n");
        return;
    }

    // obtain condition
    char *where_pair = strtok(NULL, "\n");
    if (!where_pair || !strchr(where_pair, '=')) {
        printf("Error: Invalid WHERE clause syntax.\n");
        return;
    }

    char *where_equals = strchr(where_pair, '=');
    *where_equals = '\0';
    char *where_column = trim(where_pair);
    char *where_value = clean_quotes(trim(where_equals + 1)); 


    update_rows(table, column_name, new_value, where_column, where_value);
}

void handle_delete() {

    if (strcmp(strtok(NULL, " "), "FROM") != 0) {
        printf("Error: Missing FROM keyword.\n");
        return;
    }

    char *table_name = strtok(NULL, " ");
    Table *table = find_table(table_name);
    if (!table) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    if (strcmp(strtok(NULL, " "), "WHERE") != 0) {
        printf("Error: Missing WHERE clause.\n");
        return;
    }

    char *pair = strtok(NULL, "\n");
    char *equals = strchr(pair, '=');
    if (!equals) {
        printf("Error: Invalid WHERE clause syntax.\n");
        return;
    }

    *equals = '\0';
    char *where_column = trim(pair);
    char *where_value = clean_quotes(trim(equals + 1));

    int rows_deleted = delete_rows(table, where_column, where_value);

    if (rows_deleted == 0) {
        printf("No rows found where '%s' = '%s'.\n", where_column, where_value);
    } else {
        printf("Rows deleted successfully.\n");
    }
}


char *clean_quotes(char *str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    if ((str[0] == '\'' && str[len - 1] == '\'') || (str[0] == '"' && str[len - 1] == '"')) {
        str[len - 1] = '\0'; 
        str++;               
    }
    return str;
}