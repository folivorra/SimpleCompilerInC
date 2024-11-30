#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Узлы синтаксического дерева
typedef enum {
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_CONDITION,
    NODE_LOOP,
    NODE_INPUT,
    NODE_OUTPUT,
    NODE_EXPRESSION
} NodeType;

typedef struct Node {
    NodeType type;
    char value[MAX_TOKEN_SIZE];
    struct Node *children[10];
    int child_count;
} Node;

// Структура парсера
typedef struct {
    Token *tokens;
    int current;
    int token_count;
} Parser;

// Функции парсера
Node *parse_program(Parser *parser);
Node *parse_declaration(Parser *parser);

#endif
