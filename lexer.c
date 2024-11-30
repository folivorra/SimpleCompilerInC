#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char *keywords[] = {"целое", "строка", "ассоц_массив", "если", "иначе", "конец", "цикл", "ввод", "вывод"};
const char *operators[] = {"больше", "меньше", "равно"};

int is_keyword(const char *word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_operator(const char *word) {
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        if (strcmp(word, operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void tokenize_line(const char *line, int line_number, Token *tokens, int *token_count) {
    const char *delimiters = " \t\n;";
    char buffer[1024];
    strcpy(buffer, line);

    char *word = strtok(buffer, delimiters);
    while (word != NULL) {
        Token token;
        token.line = line_number;

        if (is_keyword(word)) {
            token.type = TOKEN_KEYWORD;
        } else if (is_operator(word)) {
            token.type = TOKEN_OPERATOR;
        } else if (isdigit(word[0])) {
            token.type = TOKEN_NUMBER;
        } else if (word[0] == '"' && word[strlen(word) - 1] == '"') {
            token.type = TOKEN_STRING;
        } else if (isalpha(word[0]) || (unsigned char)word[0] >= 192) {
            token.type = TOKEN_IDENTIFIER;
        } else {
            token.type = TOKEN_UNKNOWN;
        }

        strncpy(token.value, word, MAX_TOKEN_SIZE);
        tokens[(*token_count)++] = token;

        word = strtok(NULL, delimiters);
    }
}
