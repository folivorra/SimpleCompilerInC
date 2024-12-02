// symbol_table.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

#define MAX_VARIABLES 100
#define MAX_NAME_LEN 50

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_MAP
    // ... другие типы при необходимости
} VarType;

typedef struct {
    char name[MAX_NAME_LEN];
    VarType type;
} Symbol;

typedef struct {
    Symbol symbols[MAX_VARIABLES];
    int count;
} SymbolTable;

// Функции для работы с таблицей символов
void init_symbol_table(SymbolTable *table);
int add_symbol(SymbolTable *table, const char *name, VarType type);
VarType get_symbol_type(SymbolTable *table, const char *name);
int is_symbol_declared(SymbolTable *table, const char *name);

#endif // SYMBOL_TABLE_H
