#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_SIZE 100
#define MAX_TOKENS 1000

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

// Функция для лексического анализа строки
void tokenize_line(const char *line, int line_number, Token *tokens, int *token_count);

// Проверка ключевых слов и операторов
int is_keyword(const char *word);
int is_operator(const char *word);

#endif
