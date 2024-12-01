#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// Максимальная длина токена и максимальное количество токенов
#define MAX_TOKENS 1000
#define MAX_TOKEN_LEN 100

// Лексический анализатор
int tokenize(const char *line, Token *tokens);

#endif // LEXER_H
