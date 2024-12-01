#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 100
#define MAX_TOKENS 1000

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
    TOKEN_EQ,             // ==
    TOKEN_NE,             // !=
    TOKEN_GT,             // >
    TOKEN_LT,             // <
    TOKEN_GE,             // >=
    TOKEN_LE,             // <=
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING_LITERAL,
    TOKEN_OPEN_BRACE,     // {
    TOKEN_CLOSE_BRACE,    // }
    TOKEN_SEMICOLON,
    TOKEN_OPEN_BRACKET,   // [
    TOKEN_CLOSE_BRACKET,  // ]
    TOKEN_OPEN_PAREN,     // (
    TOKEN_CLOSE_PAREN,    // )
    TOKEN_UNKNOWN
} TokenType;

// Узлы дерева синтаксиса (AST)
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

// Структура токена
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

// Узел AST
typedef struct ASTNode {
    NodeType type;
    char value[MAX_TOKEN_LEN];
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next; // Для списка операторов
} ASTNode;

// Прототипы функций парсера
ASTNode *parse_statement(Token *tokens, int *pos, int token_count);
ASTNode *parse_block(Token *tokens, int *pos, int token_count);
ASTNode *parse_expression(Token *tokens, int *pos, int token_count);
ASTNode *parse_assignment(Token *tokens, int *pos, int token_count);
ASTNode *parse_declaration(Token *tokens, int *pos, int token_count);
ASTNode *parse_if_else(Token *tokens, int *pos, int token_count);

// ===== ЛЕКСЕР =====

// Функция для определения типа токена
TokenType identify_token(const char *word) {
    if (strcmp(word, "целое") == 0) return TOKEN_INT;
    if (strcmp(word, "строка") == 0) return TOKEN_STRING;
    if (strcmp(word, "карта") == 0) return TOKEN_MAP;
    if (strcmp(word, "если") == 0) return TOKEN_IF;
    if (strcmp(word, "иначе") == 0) return TOKEN_ELSE;
    if (strcmp(word, "пока") == 0) return TOKEN_WHILE;
    if (strcmp(word, "вывод") == 0) return TOKEN_PRINT;
    if (strcmp(word, "ввод") == 0) return TOKEN_INPUT;
    if (strcmp(word, "=") == 0) return TOKEN_ASSIGN;
    if (strcmp(word, "==") == 0) return TOKEN_EQ;
    if (strcmp(word, "!=") == 0) return TOKEN_NE;
    if (strcmp(word, ">") == 0) return TOKEN_GT;
    if (strcmp(word, "<") == 0) return TOKEN_LT;
    if (strcmp(word, ">=") == 0) return TOKEN_GE;
    if (strcmp(word, "<=") == 0) return TOKEN_LE;
    if (strcmp(word, "{") == 0) return TOKEN_OPEN_BRACE;
    if (strcmp(word, "}") == 0) return TOKEN_CLOSE_BRACE;
    if (strcmp(word, "(") == 0) return TOKEN_OPEN_PAREN;
    if (strcmp(word, ")") == 0) return TOKEN_CLOSE_PAREN;
    if (strcmp(word, "[") == 0) return TOKEN_OPEN_BRACKET;
    if (strcmp(word, "]") == 0) return TOKEN_CLOSE_BRACKET;
    if (strcmp(word, ";") == 0) return TOKEN_SEMICOLON;

    // Попытка интерпретировать как число
    if (isdigit((unsigned char)word[0]) || (word[0] == '-' && isdigit((unsigned char)word[1]))) {
        return TOKEN_NUMBER;
    }

    // Попытка интерпретировать как строковый литерал
    if (word[0] == '"' && word[strlen(word) - 1] == '"') {
        return TOKEN_STRING_LITERAL;
    }

    return TOKEN_IDENTIFIER;
}

// Лексический анализатор
int tokenize(const char *line, Token *tokens) {
    int token_count = 0;
    int length = strlen(line);
    char buffer[MAX_TOKEN_LEN];
    int buffer_pos = 0;
    int inside_string = 0;

    for (int i = 0; i < length; i++) {
        char c = line[i];

        // Если мы внутри строки
        if (inside_string) {
            buffer[buffer_pos++] = c;
            if (c == '"') { // Закрывающая кавычка
                buffer[buffer_pos] = '\0';
                tokens[token_count].type = TOKEN_STRING_LITERAL;
                strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
                token_count++;
                buffer_pos = 0;
                inside_string = 0;
            }
            continue;
        }

        // Открывающая кавычка строки
        if (c == '"') {
            if (buffer_pos > 0) { // Завершаем предыдущий токен
                buffer[buffer_pos] = '\0';
                tokens[token_count].type = identify_token(buffer);
                strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
                token_count++;
                buffer_pos = 0;
            }
            inside_string = 1;
            buffer[buffer_pos++] = c;
            continue;
        }

        // Обработка двухсимвольных операторов
        if ((c == '>' || c == '<' || c == '=' || c == '!') && line[i + 1] == '=') {
            if (buffer_pos > 0) {
                buffer[buffer_pos] = '\0';
                tokens[token_count].type = identify_token(buffer);
                strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
                token_count++;
                buffer_pos = 0;
            }
            buffer[0] = c;
            buffer[1] = '=';
            buffer[2] = '\0';
            tokens[token_count].type = identify_token(buffer);
            strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
            token_count++;
            i++; // Пропускаем следующий символ '='
            continue;
        }

        // Обработка односимвольных операторов
        if (strchr("=;{}[]()<>", c) != NULL) {
            if (buffer_pos > 0) {
                buffer[buffer_pos] = '\0';
                tokens[token_count].type = identify_token(buffer);
                strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
                token_count++;
                buffer_pos = 0;
            }
            buffer[0] = c;
            buffer[1] = '\0';
            tokens[token_count].type = identify_token(buffer);
            strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
            token_count++;
            continue;
        }

        // Пропуск пробелов
        if (isspace((unsigned char)c)) {
            if (buffer_pos > 0) {
                buffer[buffer_pos] = '\0';
                tokens[token_count].type = identify_token(buffer);
                strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
                token_count++;
                buffer_pos = 0;
            }
            continue;
        }

        // Добавляем символ в текущий буфер
        buffer[buffer_pos++] = c;
    }

    // Добавляем последний токен, если он есть
    if (buffer_pos > 0) {
        buffer[buffer_pos] = '\0';
        tokens[token_count].type = identify_token(buffer);
        strncpy(tokens[token_count].value, buffer, MAX_TOKEN_LEN - 1);
        token_count++;
    }

    return token_count;
}

// ===== СИНТАКСИЧЕСКИЙ АНАЛИЗАТОР =====

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

// Парсинг выражения
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

// Парсинг присваивания
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

// Парсинг объявления переменных
ASTNode *parse_declaration(Token *tokens, int *pos, int token_count) {
    if (*pos + 1 < token_count &&
        (tokens[*pos].type == TOKEN_INT || tokens[*pos].type == TOKEN_STRING || tokens[*pos].type == TOKEN_MAP)) {
        Token *type_token = &tokens[*pos];
        Token *identifier_token = &tokens[*pos + 1];

        int new_pos = *pos + 2; // После типа и имени переменной

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

// Парсинг конструкции 'если'
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

    // Ожидаем '{'
    if (*pos >= token_count || tokens[*pos].type != TOKEN_OPEN_BRACE) {
        printf("Ошибка: ожидается '{' для тела 'если' на позиции %d\n", *pos);
        return NULL;
    }
    (*pos)++; // Пропускаем '{'

    ASTNode *if_body = parse_block(tokens, pos, token_count);

    ASTNode *if_node = create_node(NODE_IF, "если");
    if_node->left = condition;
    if_node->right = if_body;

    // Проверяем наличие 'иначе'
    if (*pos < token_count && tokens[*pos].type == TOKEN_ELSE) {
        (*pos)++; // Пропускаем 'иначе'

        if (*pos >= token_count || tokens[*pos].type != TOKEN_OPEN_BRACE) {
            printf("Ошибка: ожидается '{' для тела 'иначе' на позиции %d\n", *pos);
            return NULL;
        }
        (*pos)++; // Пропускаем '{'

        ASTNode *else_body = parse_block(tokens, pos, token_count);

        ASTNode *else_node = create_node(NODE_ELSE, "иначе");
        else_node->left = else_body;

        if_node->next = else_node;
    }

    return if_node;
}

// Парсинг блока кода
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

// Парсинг оператора или выражения
ASTNode *parse_statement(Token *tokens, int *pos, int token_count) {
    ASTNode *statement = NULL;

    if (tokens[*pos].type == TOKEN_IF) {
        statement = parse_if_else(tokens, pos, token_count);
    } else if (tokens[*pos].type == TOKEN_INT || tokens[*pos].type == TOKEN_STRING || tokens[*pos].type == TOKEN_MAP) {
        statement = parse_declaration(tokens, pos, token_count);
    } else if (tokens[*pos].type == TOKEN_IDENTIFIER) {
        statement = parse_assignment(tokens, pos, token_count);
    } else {
        printf("Ошибка: неизвестный токен \"%s\" на позиции %d\n", tokens[*pos].value, *pos);
        (*pos)++;
    }

    return statement;
}

// Главный синтаксический анализатор
ASTNode *parse(Token *tokens, int token_count) {
    int pos = 0;
    ASTNode *root = NULL;
    ASTNode *current = NULL;

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

// ===== ГЕНЕРАТОР КОДА =====

void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

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
                    node = node->next; // Перемещаем указатель на узел 'иначе', чтобы избежать повторной обработки
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
// ===== ГЛАВНАЯ ПРОГРАММА =====
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <файл_с_кодом>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return 1;
    }

    char line[256];
    Token tokens[MAX_TOKENS];
    int token_count = 0;

    while (fgets(line, sizeof(line), file)) {
        token_count += tokenize(line, tokens + token_count);
    }

    fclose(file);

    printf("Лексер: распознано %d токенов\n", token_count);

    ASTNode *ast = parse(tokens, token_count);

    if (ast == NULL) {
        printf("Ошибка: не удалось построить AST\n");
        return 1;
    }

    printf("\nПостроенное AST:\n");
    print_ast(ast, 0);

    printf("\nСгенерированный код на C:\n");
    printf("#include \"map.h\"\n");
    printf("#include <stdio.h>\n\n");
    printf("int main() {\n");
    generate_code(ast, 1);
    printf("    return 0;\n");
    printf("}\n");

    return 0;
}
