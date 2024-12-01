#include <stdio.h>
#include <string.h>
#include "generator.h"
#include "ast.h"

// Функция для вывода отступов
void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

// Функция для генерации выражения
void generate_expression(ASTNode *node) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_VARIABLE:
        case NODE_CONSTANT:
            printf("%s", node->value);
            break;
        case NODE_EQ:
        case NODE_NE:
        case NODE_GT:
        case NODE_LT:
        case NODE_GE:
        case NODE_LE:
            generate_expression(node->left);
            printf(" %s ", node->value); // Используем значение узла для оператора
            generate_expression(node->right);
            break;
        default:
            printf("/* Неизвестное выражение */");
            break;
    }
}

// Функция для генерации кода из AST
void generate_code(ASTNode *node, int level) {
    while (node != NULL) {
        switch (node->type) {
            case NODE_DECLARATION: {
                print_indent(level);
                if (strcmp(node->value, "карта") == 0) {
                    const char *size = node->right ? node->right->value : "10";
                    printf("map %s[%s];\n", node->left->value, size);
                } else if (strcmp(node->value, "целое") == 0) {
                    printf("int %s", node->left->value);
                    if (node->right && node->right->type == NODE_ASSIGNMENT) {
                        printf(" = ");
                        generate_expression(node->right->right);
                    }
                    printf(";\n");
                } else if (strcmp(node->value, "строка") == 0) {
                    printf("char* %s", node->left->value);
                    if (node->right && node->right->type == NODE_ASSIGNMENT) {
                        printf(" = ");
                        generate_expression(node->right->right);
                    }
                    printf(";\n");
                }
                break;
            }
            case NODE_ASSIGNMENT: {
                print_indent(level);
                printf("%s = ", node->left->value);
                generate_expression(node->right);
                printf(";\n");
                break;
            }
            case NODE_IF: {
                print_indent(level);
                printf("if (");
                generate_expression(node->left);
                printf(") {\n");
                generate_code(node->right, level + 1); // Генерация тела условия
                print_indent(level);
                printf("}");
                if (node->next && node->next->type == NODE_ELSE) {
                    printf(" else {\n");
                    generate_code(node->next->left, level + 1); // Генерация тела 'иначе'
                    print_indent(level);
                    printf("}\n");
                    node = node->next; // Перемещаем указатель на узел 'иначе'
                } else {
                    printf("\n");
                }
                break;
            }
            case NODE_ELSE: {
                // На случай, если узел 'иначе' появится вне 'если'
                print_indent(level);
                printf("else {\n");
                generate_code(node->left, level + 1);
                print_indent(level);
                printf("}\n");
                break;
            }
            default:
                print_indent(level);
                printf("/* Неизвестный узел типа %d */\n", node->type);
                break;
        }
        node = node->next;
    }
}
