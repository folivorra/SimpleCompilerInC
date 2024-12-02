// symbol_table.c
#include <string.h>
#include "symbol_table.h"

// Инициализация таблицы символов
void init_symbol_table(SymbolTable *table) {
    table->count = 0;
}

// Добавление символа в таблицу символов
int add_symbol(SymbolTable *table, const char *name, VarType type) {
    if (table->count >= MAX_VARIABLES) {
        return -1; // Таблица заполнена
    }

    // Проверка на дублирование
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return -2; // Переменная уже объявлена
        }
    }

    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].type = type;
    table->count++;
    return 0; // Успешно добавлено
}

// Получение типа символа по имени
VarType get_symbol_type(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return table->symbols[i].type;
        }
    }
    // Возвращаем тип по умолчанию, если переменная не найдена
    return TYPE_INT;
}

// Проверка, объявлена ли переменная
int is_symbol_declared(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return 1; // Объявлена
        }
    }
    return 0; // Не объявлена
}
