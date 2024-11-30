#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_SIZE 100
#define MAX_LINE_LENGTH 1024

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_SIZE];
    int line;
} Token;

const char *keywords[] = {
    "целое", "строка", "ассоц_массив",
    "если", "иначе", "конец", "цикл",
    "ввод", "вывод"
};

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
    char buffer[MAX_LINE_LENGTH];
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

void print_tokens(const Token *tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        printf("Line %d: [%s] (%d)\n", tokens[i].line, tokens[i].value, tokens[i].type);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    Token tokens[1000];
    int token_count = 0;

    char line[MAX_LINE_LENGTH];
    int line_number = 1;

    while (fgets(line, sizeof(line), file)) {
        tokenize_line(line, line_number++, tokens, &token_count);
    }

    fclose(file);

    print_tokens(tokens, token_count);

    return 0;
}