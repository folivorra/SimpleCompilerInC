#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define MAX_TOKEN_LEN 100

// Функция для создания узла AST
ASTNode *create_node(NodeType type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    strncpy(node->value, value ? value : "", MAX_TOKEN_LEN);
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

// Вывод AST
void print_ast(ASTNode *node, int level) {
    while (node != NULL) {
        for (int i = 0; i < level; i++) printf("  ");
        printf("Тип узла: %d, Значение: %s\n", node->type, node->value);
        if (node->left) print_ast(node->left, level + 1);
        if (node->right) print_ast(node->right, level + 1);
        node = node->next;
    }
}