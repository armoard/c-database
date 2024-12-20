# Simple Database in C
I wanted to explore how databases work under the hood, so I built a simple database system in C. It uses fundamental data structures like hash tables and dynamic arrays to handle operations efficiently.
Currently, it supports basic functionality like creating tables and adding columns,inserting, updating,deleting rows and querying data

Maybe in the future i can add more advanced features, such as implementing B-Trees for better indexing and a more robust query parser to handle input.


## Requirements
You only need GCC (GNU Compiler Collection) for this proyect.

## Compilation
To compile the project without a Makefile, use the following command: 

    gcc -Wall -Wextra -Wno-unused-value -Iinclude -Ivendor/hash -Ivendor/vector src/main.c src/database.c src/parser.c vendor/hash/hash.c vendor/vector/vector.c -o database.exe

Then:

    ./database.exe

If using the provided Makefile, simply run:

    make run

## Available Commands


    CREATE TABLE <name>	Creates a new table with the specified name.
    ADD COLUMN <table> <column_name>	Adds a new column to an existing table.
    INSERT INTO <table> column='value'	Inserts data into an existing table.
    SELECT columns WHERE <table> condition	Queries rows meeting a condition.
    UPDATE <table> SET column='value'	Updates values in specific rows.
    DELETE FROM <table> WHERE column=value	Deletes rows that meet a condition.
    LIST TABLES	Lists all existing tables.
    LIST COLUMNS <table>	Displays all columns of a specific table.
    LIST ROWS <table>	Displays all rows of a specific table.
    EXIT	Exits the program.


## Example command flow  

    Simple Database Shell. Type 'exit' to quit.
    db> CREATE TABLE users
    Table 'users' created.
    db> ADD COLUMN users first_name
    Column 'first_name' added to table 'users'.
    db> ADD COLUMN users last_name
    Column 'last_name' added to table 'users'.
    db> INSERT INTO users first_name='Bruce' last_name='Wayne'
    Row inserted into table 'users'.
    db> SELECT first_name, last_name WHERE users last_name='Wayne'
    first_name: Bruce
    last_name: Wayne
    db> LIST ROWS users
    Rows in table 'users':
    Row 1:
    first_name: Bruce
    last_name: Wayne
    db> EXIT

## Notes

-Data is stored in-memory only. There is no persistence.

-Supports up to 10 columns per row (adjustable in the code).

-This project uses third-party libraries stored in the vendor directory. These libraries are not written or maintained by me. They are included to provide functionality required for the project.

Hash Library: Located in vendor/hash

Vector Library: Located in vendor/vector
