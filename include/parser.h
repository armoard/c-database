#ifndef PARSER_H
#define PARSER_H

void parse_command(char *input);
void handle_create_table();
void handle_drop_table();
void handle_add_column();
void handle_insert_into();
void handle_list();
void handle_select();
void handle_delete();
void handle_list_rows();
void handle_update();
char *clean_quotes(char *str);
#endif /* PARSER_H */