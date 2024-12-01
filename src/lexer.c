#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "token.h"

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
