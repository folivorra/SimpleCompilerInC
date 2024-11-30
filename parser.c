#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *new_node(NodeType type, const char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    if (value) {
        strcpy(node->value, value);
    }
    node->child_count = 0;
    return node;
}

Token current_token(Parser *parser) {
    return parser->tokens[parser->current];
}

int match(Parser *parser, const char *expected) {
    if (parser->current < parser->token_count &&
        strcmp(current_token(parser).value, expected) == 0) {
        parser->current++;
        return 1;
    }
    return 0;
}

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

Node *parse_program(Parser *parser) {
    Node *root = new_node(NODE_PROGRAM, "");
    while (parser->current < parser->token_count) {
        Node *node = parse_declaration(parser);
        if (!node) {
            printf("Syntax error: Unknown syntax (line %d)\n", current_token(parser).line);
            exit(1);
        }
        root->children[root->child_count++] = node;
    }
    return root;
}
