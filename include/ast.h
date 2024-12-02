#ifndef AST_H
#define AST_H

#include "token.h"

#define MAX_TOKEN_LEN 100

// Типы узлов AST
typedef enum {
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_ELSE,
    NODE_WHILE,
    NODE_PRINT,
    NODE_INPUT,
    NODE_VARIABLE,
    NODE_CONSTANT,
    NODE_DECLARATION,
    NODE_EQ,        // Оператор ==
    NODE_NE,        // Оператор !=
    NODE_GT,        // Оператор >
    NODE_LT,        // Оператор <
    NODE_GE,        // Оператор >=
    NODE_LE         // Оператор <=
} NodeType;

// Узел AST
typedef struct ASTNode {
    NodeType type;
    char value[MAX_TOKEN_LEN];
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next; // Для списка операторов
} ASTNode;

// Функции для работы с AST
ASTNode *create_node(NodeType type, const char *value);
void print_ast(ASTNode *node, int level);

#endif // AST_H
