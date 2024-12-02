// generator.c
#include <stdio.h>
#include <string.h>
#include "generator.h"
#include "ast.h"
#include "symbol_table.h"

// Глобальная таблица символов (должна быть доступна из parser.c)
extern SymbolTable sym_table;

// Функция для вывода отступов
void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

// Функция для получения спецификатора формата
const char* get_format_specifier(const char *var_name) {
    VarType type = get_symbol_type(&sym_table, var_name);
    switch (type) {
        case TYPE_INT:
            return "%u";
        case TYPE_STRING:
            return "%s";
        case TYPE_MAP:
            // Предполагаем, что карта представлена как массив, но для ввода/вывода нужно определить
            // Для простоты используем %u
            return "%u";
        default:
            return "%d"; // По умолчанию
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
                    printf("unsigned int %s", node->left->value);
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
            case NODE_INPUT: {
                print_indent(level);
                printf("scanf(\"%s\", &%s);\n", get_format_specifier(node->value), node->value);
                break;
            }
            case NODE_PRINT: {
                print_indent(level);
                printf("printf(\"%s\\n\", %s);\n", get_format_specifier(node->value), node->value);
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
