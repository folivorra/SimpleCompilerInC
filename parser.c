#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Создание нового узла дерева
Node *new_node(NodeType type, const char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    if (value) {
        strcpy(node->value, value);
    } else {
        node->value[0] = '\0';
    }
    node->child_count = 0;
    return node;
}

// Получение текущего токена
Token current_token(Parser *parser) {
    return parser->tokens[parser->current];
}

// Сравнение текущего токена с ожидаемым значением
int match(Parser *parser, const char *expected) {
    if (parser->current < parser->token_count &&
        strcmp(current_token(parser).value, expected) == 0) {
        parser->current++;
        return 1;
    }
    return 0;
}

// Парсинг объявления переменной
Node *parse_declaration(Parser *parser) {
    if (match(parser, "целое") || match(parser, "строка") || match(parser, "ассоц_массив")) {
        Token token = current_token(parser);
        if (token.type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected identifier after type declaration (line %d)\n", token.line);
            exit(1);
        }
        parser->current++;
        return new_node(NODE_DECLARATION, token.value);
    }
    return NULL;
}

// Парсинг оператора ввода
Node *parse_input(Parser *parser) {
    if (match(parser, "ввод")) {
        Token token = current_token(parser);
        if (token.type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected identifier after 'ввод' (line %d)\n", token.line);
            exit(1);
        }
        parser->current++;
        return new_node(NODE_INPUT, token.value);
    }
    return NULL;
}

// Парсинг оператора вывода
Node *parse_output(Parser *parser) {
    if (match(parser, "вывод")) {
        Token token = current_token(parser);
        if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_STRING) {
            parser->current++;
            return new_node(NODE_OUTPUT, token.value);
        } else {
            printf("Syntax error: Expected identifier or string after 'вывод' (line %d)\n", token.line);
            exit(1);
        }
    }
    return NULL;
}

// Парсинг выражения
Node *parse_expression(Parser *parser) {
    Token token = current_token(parser);
    if (token.type == TOKEN_IDENTIFIER) {
        parser->current++;
        Token op = current_token(parser);
        if (op.type != TOKEN_OPERATOR) {
            printf("Syntax error: Expected operator in expression (line %d)\n", op.line);
            exit(1);
        }
        parser->current++;
        Token right = current_token(parser);
        if (right.type != TOKEN_NUMBER && right.type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected number or identifier in expression (line %d)\n", right.line);
            exit(1);
        }
        parser->current++;
        return new_node(NODE_EXPRESSION, "");
    }
    printf("Syntax error: Invalid expression (line %d)\n", token.line);
    exit(1);
}

// Парсинг условия
Node *parse_condition(Parser *parser) {
    if (match(parser, "если")) {
        Node *node = new_node(NODE_CONDITION, "");
        Node *expr = parse_expression(parser);
        if (!match(parser, "то")) {
            printf("Syntax error: Expected 'то' after condition (line %d)\n", current_token(parser).line);
            exit(1);
        }
        node->children[node->child_count++] = expr;

        // Парсим блок "то"
        while (!match(parser, "иначе") && !match(parser, "конец")) {
            Node *child = parse_program(parser);
            node->children[node->child_count++] = child;
        }

        // Если есть "иначе", парсим его блок
        if (current_token(parser).type == TOKEN_KEYWORD &&
            strcmp(current_token(parser).value, "иначе") == 0) {
            parser->current++;
            while (!match(parser, "конец")) {
                Node *child = parse_program(parser);
                node->children[node->child_count++] = child;
            }
        }

        return node;
    }
    return NULL;
}


// Парсинг цикла
Node *parse_loop(Parser *parser) {
    if (match(parser, "цикл")) {
        Node *node = new_node(NODE_LOOP, "");
        Node *expr = parse_expression(parser);
        node->children[node->child_count++] = expr;

        // Парсим тело цикла
        while (!match(parser, "конец")) {
            Node *child = parse_program(parser);
            node->children[node->child_count++] = child;
        }

        return node;
    }
    return NULL;
}

// Парсинг программы
Node *parse_program(Parser *parser) {
    Node *node = NULL;

    // Проверяем тип конструкции
    if ((node = parse_declaration(parser)) ||
        (node = parse_input(parser)) ||
        (node = parse_output(parser)) ||
        (node = parse_condition(parser)) ||
        (node = parse_loop(parser))) {
        return node;
    }

    printf("Syntax error: Unknown syntax (line %d)\n", current_token(parser).line);
    exit(1);
}