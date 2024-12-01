#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

// Функции парсера
ASTNode *parse(Token *tokens, int token_count);

#endif // PARSER_H
