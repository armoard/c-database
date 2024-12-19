#include "hash.h"
#include "vector.h"

#ifndef DATABASE_H
#define DATABASE_H


typedef struct Column {
  char *name;
} Column;

typedef struct Row {
  hash_t *cells; // col name -> string value 
} Row;

typedef vec_t(Row*) row_vec_t;

typedef struct Table {
  char *name;
  hash_t *columns;
  row_vec_t *rows;
} Table;


void init_db();
void free_db();
int find_table_index(char *table_name);
Table *find_table(char *table_name);
Column *find_column(Table *table, char *column_name);
int create_table(char *name);
int drop_table(Table *table);
int add_column(Table *table, char *column_name);
int drop_column(Table *table, Column *column);
int drop_row(Table *table, Row *row);
int insert_into(Table *table, char *column_names[], char *values[], int num_values);
hash_t *select_from(Table *table, char *column_names[], int num_columns, int row_index);
int update_rows(Table *table, char *column_name, char *new_value, char *where_column, char *where_value);
int delete_rows(Table *table, char *where_column, char *where_value);
int list_columns(Table *table);
int list_tables();
void list_rows(Table *table);
int where_equals(char *table_name, char *column_name, char *value);
char *trim(char *str);

#endif