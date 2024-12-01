#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_LINE_LEN 1024

// Типы токенов
typedef enum {
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_MAP,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_PRINT,
    TOKEN_INPUT,
    TOKEN_ASSIGN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING_LITERAL,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_UNKNOWN
} TokenType;

// Структура токена
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

#endif