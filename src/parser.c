#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "token.h"
#include "symbol_table.h"

// Функции парсера

ASTNode *parse_input(Token *tokens, int *pos, int token_count);
ASTNode *parse_print(Token *tokens, int *pos, int token_count);

// Парсинг выражения
ASTNode *parse_expression(Token *tokens, int *pos, int token_count);

// Парсинг присваивания
ASTNode *parse_assignment(Token *tokens, int *pos, int token_count);

// Парсинг объявления переменных
ASTNode *parse_declaration(Token *tokens, int *pos, int token_count);

// Парсинг конструкции 'если'
ASTNode *parse_if_else(Token *tokens, int *pos, int token_count);

// Парсинг блока кода
ASTNode *parse_block(Token *tokens, int *pos, int token_count);

// Парсинг оператора или выражения
ASTNode *parse_statement(Token *tokens, int *pos, int token_count);

// Глобальная таблица символов
SymbolTable sym_table;

// Главный синтаксический анализатор
ASTNode *parse(Token *tokens, int token_count) {
    int pos = 0;
    ASTNode *root = NULL;
    ASTNode *current = NULL;

    // Инициализируем таблицу символов
    init_symbol_table(&sym_table);

    while (pos < token_count) {
        ASTNode *statement = parse_statement(tokens, &pos, token_count);
        if (statement) {
            if (root == NULL) {
                root = statement;
                current = root;
            } else {
                current->next = statement;
                current = statement;
            }
        } else {
            // Если парсер не смог разобрать выражение, пропускаем токен
            pos++;
        }
    }

    return root;
}

// Обновленная функция парсинга операторов или выражений
ASTNode *parse_statement(Token *tokens, int *pos, int token_count) {
    ASTNode *statement = NULL;

    if (tokens[*pos].type == TOKEN_IF) {
        statement = parse_if_else(tokens, pos, token_count);
    } 
    else if (tokens[*pos].type == TOKEN_PRINT) {
        statement = parse_print(tokens, pos, token_count);
    }
    else if (tokens[*pos].type == TOKEN_INPUT) {
        statement = parse_input(tokens, pos, token_count);
    }
    else if (tokens[*pos].type == TOKEN_INT || tokens[*pos].type == TOKEN_STRING || tokens[*pos].type == TOKEN_MAP) {
        statement = parse_declaration(tokens, pos, token_count);
    } 
    else if (tokens[*pos].type == TOKEN_IDENTIFIER) {
        statement = parse_assignment(tokens, pos, token_count);
    } 
    else {
        printf("Ошибка: неизвестный токен \"%s\" на позиции %d\n", tokens[*pos].value, *pos);
        (*pos)++;
    }

    return statement;
}

// Функция парсинга оператора ввода
ASTNode *parse_input(Token *tokens, int *pos, int token_count) {
    if (*pos >= token_count || tokens[*pos].type != TOKEN_INPUT) {
        return NULL;
    }

    (*pos)++; // Пропускаем 'ввод'

    if (*pos >= token_count || tokens[*pos].type != TOKEN_IDENTIFIER) {
        printf("Ошибка: ожидается идентификатор после 'ввод' на позиции %d\n", *pos);
        return NULL;
    }

    Token *identifier_token = &tokens[*pos];
    (*pos)++; // Пропускаем идентификатор

    if (*pos >= token_count || tokens[*pos].type != TOKEN_SEMICOLON) {
        printf("Ошибка: ожидается ';' после оператора 'ввод' на позиции %d\n", *pos);
        return NULL;
    }

    (*pos)++; // Пропускаем ';'

    // Проверяем, объявлена ли переменная
    if (!is_symbol_declared(&sym_table, identifier_token->value)) {
        printf("Ошибка: переменная '%s' не объявлена перед использованием 'ввод' на позиции %d\n", identifier_token->value, *pos);
        return NULL;
    }

    // Создаем узел AST для оператора ввода
    ASTNode *input_node = create_node(NODE_INPUT, identifier_token->value);
    return input_node;
}

// Функция парсинга оператора вывода
ASTNode *parse_print(Token *tokens, int *pos, int token_count) {
    if (*pos >= token_count || tokens[*pos].type != TOKEN_PRINT) {
        return NULL;
    }

    (*pos)++; // Пропускаем 'вывод'

    if (*pos >= token_count || tokens[*pos].type != TOKEN_IDENTIFIER) {
        printf("Ошибка: ожидается идентификатор после 'вывод' на позиции %d\n", *pos);
        return NULL;
    }

    Token *identifier_token = &tokens[*pos];
    (*pos)++; // Пропускаем идентификатор

    if (*pos >= token_count || tokens[*pos].type != TOKEN_SEMICOLON) {
        printf("Ошибка: ожидается ';' после оператора 'вывод' на позиции %d\n", *pos);
        return NULL;
    }

    (*pos)++; // Пропускаем ';'

    // Проверяем, объявлена ли переменная
    if (!is_symbol_declared(&sym_table, identifier_token->value)) {
        printf("Ошибка: переменная '%s' не объявлена перед использованием 'вывод' на позиции %d\n", identifier_token->value, *pos);
        return NULL;
    }

    // Создаем узел AST для оператора вывода
    ASTNode *print_node = create_node(NODE_PRINT, identifier_token->value);
    return print_node;
}

// Функция парсинга объявления переменной с добавлением в таблицу символов
ASTNode *parse_declaration(Token *tokens, int *pos, int token_count) {
    if (*pos + 1 < token_count &&
        (tokens[*pos].type == TOKEN_INT || tokens[*pos].type == TOKEN_STRING || tokens[*pos].type == TOKEN_MAP)) {
        Token *type_token = &tokens[*pos];
        Token *identifier_token = &tokens[*pos + 1];

        int new_pos = *pos + 2; // После типа и имени переменной

        // Определение типа переменной
        VarType var_type;
        if (type_token->type == TOKEN_INT) {
            var_type = TYPE_INT;
        } else if (type_token->type == TOKEN_STRING) {
            var_type = TYPE_STRING;
        } else if (type_token->type == TOKEN_MAP) {
            var_type = TYPE_MAP;
        } else {
            var_type = TYPE_INT; // Тип по умолчанию
        }

        // Добавляем переменную в таблицу символов
        int add_result = add_symbol(&sym_table, identifier_token->value, var_type);
        if (add_result == -1) {
            printf("Ошибка: превышено количество переменных.\n");
            return NULL;
        } else if (add_result == -2) {
            printf("Ошибка: переменная '%s' уже объявлена.\n", identifier_token->value);
            return NULL;
        }

        ASTNode *node = create_node(NODE_DECLARATION, type_token->value);
        node->left = create_node(NODE_VARIABLE, identifier_token->value);

        // Если это карта с массивом
        if (tokens[*pos].type == TOKEN_MAP &&
            new_pos < token_count && tokens[new_pos].type == TOKEN_OPEN_BRACKET) {
            new_pos++; // Пропускаем '['
            if (new_pos < token_count && tokens[new_pos].type == TOKEN_NUMBER) {
                Token *size_token = &tokens[new_pos];
                new_pos++; // Пропускаем число
                if (new_pos < token_count && tokens[new_pos].type == TOKEN_CLOSE_BRACKET) {
                    new_pos++; // Пропускаем ']'
                    // Сохраняем размер массива в AST
                    node->right = create_node(NODE_CONSTANT, size_token->value);
                } else {
                    printf("Ошибка: ожидается ']' после размера массива на позиции %d\n", new_pos);
                    *pos = new_pos; // Обновляем позицию, чтобы избежать зацикливания
                    return NULL;
                }
            } else {
                printf("Ошибка: ожидается число после '[' на позиции %d\n", new_pos);
                *pos = new_pos; // Обновляем позицию
                return NULL;
            }
        }

        // Проверка присваивания для переменных
        if (tokens[*pos].type != TOKEN_MAP && new_pos < token_count && tokens[new_pos].type == TOKEN_ASSIGN) {
            Token *assign_token = &tokens[new_pos];
            Token *value_token = &tokens[new_pos + 1];

            if (new_pos + 1 < token_count && tokens[new_pos + 2].type == TOKEN_SEMICOLON) {
                ASTNode *assignment_node = create_node(NODE_ASSIGNMENT, assign_token->value);
                assignment_node->left = node->left;
                assignment_node->right = create_node(
                    value_token->type == TOKEN_IDENTIFIER ? NODE_VARIABLE : NODE_CONSTANT,
                    value_token->value);

                node->right = assignment_node; // Связываем присваивание с узлом объявления
                new_pos += 3; // Пропускаем '=', значение и ';'
            } else {
                printf("Ошибка: ожидается ';' после присваивания на позиции %d\n", new_pos + 2);
                *pos = new_pos + 2;
                return NULL;
            }
        } else {
            // Проверяем наличие точки с запятой после объявления
            if (new_pos < token_count && tokens[new_pos].type == TOKEN_SEMICOLON) {
                new_pos++; // Пропускаем точку с запятой
            } else {
                printf("Ошибка: ожидается ';' после объявления на позиции %d\n", new_pos);
                *pos = new_pos;
                return NULL;
            }
        }

        *pos = new_pos;
        return node;
    }

    return NULL;
}

ASTNode *parse_assignment(Token *tokens, int *pos, int token_count) {
    if (*pos + 2 < token_count && tokens[*pos + 1].type == TOKEN_ASSIGN) {
        Token *identifier_token = &tokens[*pos];
        Token *assign_token = &tokens[*pos + 1];
        Token *value_token = &tokens[*pos + 2];

        int new_pos = *pos + 3; // После переменной, '=', значения

        // Проверяем наличие точки с запятой
        if (new_pos < token_count && tokens[new_pos].type == TOKEN_SEMICOLON) {
            new_pos++; // Пропускаем точку с запятой
        } else {
            printf("Ошибка: ожидается ';' после присваивания на позиции %d\n", new_pos);
            *pos = new_pos;
            return NULL;
        }

        // Создаём узел AST для присваивания
        ASTNode *node = create_node(NODE_ASSIGNMENT, assign_token->value);
        node->left = create_node(NODE_VARIABLE, identifier_token->value);
        node->right = create_node(
            value_token->type == TOKEN_IDENTIFIER ? NODE_VARIABLE : NODE_CONSTANT,
            value_token->value);

        *pos = new_pos;
        return node;
    }

    return NULL;
}

ASTNode *parse_if_else(Token *tokens, int *pos, int token_count) {
    if (*pos >= token_count || tokens[*pos].type != TOKEN_IF) {
        return NULL;
    }

    (*pos)++; // Пропускаем 'если'

    // Парсим условие
    ASTNode *condition = parse_expression(tokens, pos, token_count);
    if (!condition) {
        printf("Ошибка: неверное условие в 'если' на позиции %d\n", *pos);
        return NULL;
    }

    ASTNode *if_body = NULL;

    // Проверяем наличие '{' или одиночного выражения
    if (*pos < token_count && tokens[*pos].type == TOKEN_OPEN_BRACE) {
        (*pos)++; // Пропускаем '{'
        if_body = parse_block(tokens, pos, token_count);
    } else {
        // Парсим одно выражение как тело
        if_body = parse_statement(tokens, pos, token_count);
        if (!if_body) {
            printf("Ошибка: отсутствует тело 'если' на позиции %d\n", *pos);
            return NULL;
        }
    }

    ASTNode *if_node = create_node(NODE_IF, "если");
    if_node->left = condition;
    if_node->right = if_body;

    // Проверяем наличие 'иначе'
    if (*pos < token_count && tokens[*pos].type == TOKEN_ELSE) {
        (*pos)++; // Пропускаем 'иначе'

        ASTNode *else_body = NULL;
        if (*pos < token_count && tokens[*pos].type == TOKEN_OPEN_BRACE) {
            (*pos)++; // Пропускаем '{'
            else_body = parse_block(tokens, pos, token_count);
        } else {
            // Парсим одно выражение как тело
            else_body = parse_statement(tokens, pos, token_count);
            if (!else_body) {
                printf("Ошибка: отсутствует тело 'иначе' на позиции %d\n", *pos);
                return NULL;
            }
        }

        ASTNode *else_node = create_node(NODE_ELSE, "иначе");
        else_node->left = else_body;

        if_node->next = else_node;
    }

    return if_node;
}

ASTNode *parse_block(Token *tokens, int *pos, int token_count) {
    ASTNode *root = NULL;
    ASTNode *current = NULL;

    while (*pos < token_count && tokens[*pos].type != TOKEN_CLOSE_BRACE) {
        ASTNode *statement = parse_statement(tokens, pos, token_count);
        if (statement) {
            if (root == NULL) {
                root = statement;
                current = root;
            } else {
                current->next = statement;
                current = statement;
            }
        } else {
            // Если парсер не смог разобрать выражение, пропускаем токен
            (*pos)++;
        }
    }

    if (*pos >= token_count || tokens[*pos].type != TOKEN_CLOSE_BRACE) {
        printf("Ошибка: ожидается '}' на позиции %d\n", *pos);
        return NULL;
    }
    (*pos)++; // Пропускаем '}'

    return root;
}


ASTNode *parse_expression(Token *tokens, int *pos, int token_count) {
    ASTNode *left = NULL;

    // Парсим левый операнд (переменная или число)
    if (tokens[*pos].type == TOKEN_IDENTIFIER || tokens[*pos].type == TOKEN_NUMBER) {
        left = create_node(
            tokens[*pos].type == TOKEN_IDENTIFIER ? NODE_VARIABLE : NODE_CONSTANT,
            tokens[*pos].value);
        (*pos)++;
    } else {
        printf("Ошибка: ожидается переменная или число в выражении на позиции %d\n", *pos);
        return NULL;
    }

    // Проверяем, есть ли оператор сравнения
    if (*pos < token_count &&
        (tokens[*pos].type == TOKEN_EQ || tokens[*pos].type == TOKEN_NE ||
         tokens[*pos].type == TOKEN_GT || tokens[*pos].type == TOKEN_LT ||
         tokens[*pos].type == TOKEN_GE || tokens[*pos].type == TOKEN_LE)) {

        TokenType operator_type = tokens[*pos].type;
        const char *operator_value = tokens[*pos].value;
        (*pos)++; // Пропускаем оператор

        // Парсим правый операнд
        if (*pos < token_count && (tokens[*pos].type == TOKEN_IDENTIFIER || tokens[*pos].type == TOKEN_NUMBER)) {
            ASTNode *right = create_node(
                tokens[*pos].type == TOKEN_IDENTIFIER ? NODE_VARIABLE : NODE_CONSTANT,
                tokens[*pos].value);
            (*pos)++;

            // Создаем узел сравнения
            NodeType node_type;
            switch (operator_type) {
                case TOKEN_EQ:
                    node_type = NODE_EQ;
                    break;
                case TOKEN_NE:
                    node_type = NODE_NE;
                    break;
                case TOKEN_GT:
                    node_type = NODE_GT;
                    break;
                case TOKEN_LT:
                    node_type = NODE_LT;
                    break;
                case TOKEN_GE:
                    node_type = NODE_GE;
                    break;
                case TOKEN_LE:
                    node_type = NODE_LE;
                    break;
                default:
                    printf("Ошибка: неизвестный оператор на позиции %d\n", *pos);
                    return NULL;
            }

            ASTNode *expr_node = create_node(node_type, operator_value);
            expr_node->left = left;
            expr_node->right = right;
            return expr_node;
        } else {
            printf("Ошибка: ожидается переменная или число после оператора на позиции %d\n", *pos);
            return NULL;
        }
    }

    // Если оператора нет, возвращаем левый операнд
    return left;
}