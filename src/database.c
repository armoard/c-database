#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"

#include "database.h"

hash_t *tables;


void init_db() {
  tables = hash_new();
}

void free_db() {
  hash_each_val(tables, {
    drop_table((Table *) val);
  })
  hash_free(tables);
}

Table *find_table(char *table_name) {
  return hash_get(tables, table_name);
}

int create_table(char *name) {
    Table *table = malloc(sizeof(Table));
    table->name = strdup(name); // strtok
    table->columns = hash_new();
    table->rows = malloc(sizeof(*table->rows));
    vec_init(table->rows);
    hash_set(tables, table->name, table);

    return 0;
}

int drop_table(Table *table) {
  if (!table) {
    return -1;
  }

  // free columns 
  hash_each_val(table->columns, {
    drop_column(table, (Column *) val);
  })
  hash_free(table->columns);

  // free rows 
  // store the rows in a temp array to not modify the structure we are looping over

  int row_num = table->rows->length;
  Row *rows[row_num];

  int i;
  Row *row;
  vec_foreach(table->rows, row, i) {
    rows[i] = row;
  }
  for (i = 0; i < row_num; i++) {
    drop_row(table, rows[i]);
  }

  vec_deinit(table->rows);
  free(table->rows);

  hash_del(tables, table->name);
  free(table);

  return 0;
}

int add_column(Table *table, char *column_name) {
    if (table == NULL) {
        return -1; 
    }

    Column *column = malloc(sizeof(Column));
    column->name = strdup(column_name); 
    hash_set(table->columns, column->name, column);

    return 0;
}

int drop_column(Table *table, Column *column) {
  hash_del(table->columns, column->name);

  Row *row;
  int i;
  vec_foreach(table->rows, row, i) { 
    hash_del(row->cells, column->name);
  }

  free(column);

  return 0;
}

int drop_row(Table *table, Row *row) {
  hash_free(row->cells);
  vec_remove(table->rows, row);
  free(row);
  return 0; 
}
Column *find_column(Table *table, char *column_name) {
  return hash_get(table->columns, column_name);
}

int insert_into(Table *table, char *column_names[], char *values[], int num_values) {
    if (!table) {
        return -1;
    }

    Row *row = malloc(sizeof(Row));
    row->cells = hash_new();

    for (int i = 0; i < num_values; i++) {
        Column *column = find_column(table, column_names[i]);
        if (!column) {
            printf("Error: Column '%s' not found.\n", column_names[i]);
            hash_free(row->cells);
            free(row);
            return -1;
        }

        char *value = strdup(trim(values[i])); 
        hash_set(row->cells, column->name, value);
    }

    vec_push(table->rows, row);
    return 0;
}


hash_t *select_from(Table *table, char *column_names[], int num_columns, int row_index) {
    if (row_index == -1 || !table || row_index >= table->rows->length) {
        return NULL;
    }

    Row *row = table->rows->data[row_index];
    if (!row) return NULL;

    hash_t *result = hash_new();
    for (int i = 0; i < num_columns; i++) {
        Column *column = find_column(table, column_names[i]);
        if (!column) {
            printf("Error: Column '%s' not found.\n", column_names[i]);
            hash_free(result);
            return NULL;
        }

        char *value = hash_get(row->cells, column_names[i]);
        if (value) {
            hash_set(result, column_names[i], value);
        } else {
            printf("Warning: Column '%s' is empty in this row.\n", column_names[i]);
        }
    }

    return result;
}

int list_columns(Table *table) {
  hash_each_key(table->columns, {
    printf("%s\n", key);
  })

  return 0;
}

int list_tables() {
  hash_each(tables, {
    printf("Table %s\n", key);
    puts("COLUMNS:");
    list_columns((Table *) val);
    puts("");
  })

  return 0;
}

int where_equals(char *table_name, char *column_name, char *value) {
    Table *table = find_table(table_name);
    if (!table) {
        printf("Error: Table '%s' not found.\n", table_name);
        return -1;
    }

    Column *column = find_column(table, column_name);
    if (!column) {
        printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
        return -1;
    }

    char *clean_value = trim(value);
    int row_index = -1;
    Row *row;
    int i;

    vec_foreach(table->rows, row, i) {
        char *cell_value = hash_get(row->cells, column->name);
        if (cell_value && strcmp(cell_value, clean_value) == 0) {
            row_index = i;
            break;
        }
    }

    return row_index;
}

int delete_rows(Table *table, char *where_column, char *where_value) {
    if (!table) return -1;

    printf("Deleting rows in table '%s' where '%s' = '%s'\n", table->name, where_column, where_value);

    Column *condition_column = find_column(table, where_column);
    if (!condition_column) {
        printf("Error: Column '%s' not found.\n", where_column);
        return -1;
    }

    Row *row;
    int i;
    int deleted_count = 0;

    for (i = table->rows->length - 1; i >= 0; i--) {
        row = table->rows->data[i];
        char *cell_value = hash_get(row->cells, where_column);
        if (cell_value && strcmp(cell_value, where_value) == 0) {
            drop_row(table, row);
            deleted_count++;
        }
    }

    printf("%d rows deleted.\n", deleted_count);
    return deleted_count;
}

int update_rows(Table *table, char *column_name, char *new_value, char *where_column, char *where_value) {
    if (!table) return -1;

    printf("Updating rows in table '%s' where '%s' = '%s'\n", table->name, where_column, where_value);

    Column *update_column = find_column(table, column_name);
    Column *condition_column = find_column(table, where_column);

    if (!update_column || !condition_column) {
        printf("Error: Column not found.\n");
        return -1;
    }

    int updated_count = 0;
    Row *row;
    int i;

    vec_foreach(table->rows, row, i) {
        char *cell_value = hash_get(row->cells, where_column);
        if (cell_value && strcmp(cell_value, where_value) == 0) {
            hash_set(row->cells, column_name, strdup(new_value));
            updated_count++;
        }
    }

    printf("%d rows updated.\n", updated_count);
    return updated_count;
}


void list_rows(Table *table) {
    if (!table || table->rows->length == 0) {
        printf("The table '%s' is empty or does not exist.\n", table ? table->name : "unknown");
        return;
    }

    printf("Rows in table '%s':\n", table->name);
    Row *row;
    int i;

    vec_foreach(table->rows, row, i) {
        printf("Row %d:\n", i + 1);
        hash_each(row->cells, {
            printf("  %s: %s\n", key, (char *)val);
        });
    }
}
char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++; 
    if (*str == 0) return str; 

    char *end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '\'')) end--; 
    if (*str == '\'') str++; 

    *(end + 1) = '\0'; 
    return str;
}