#ifndef TOKEN_H
#define TOKEN_H

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

// Структура токена
typedef struct {
    TokenType type;
    char value[100]; // MAX_TOKEN_LEN
} Token;

// Функции для работы с токенами
TokenType identify_token(const char *word);

#endif // TOKEN_H
