#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

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

    Token tokens[MAX_TOKENS];
    int token_count = 0;

    char line[1024];
    int line_number = 1;

    // Лексический анализ
    while (fgets(line, sizeof(line), file)) {
        tokenize_line(line, line_number++, tokens, &token_count);
    }
    fclose(file);

    // Синтаксический анализ
    Parser parser = {tokens, 0, token_count};
    Node *root = parse_program(&parser);

    printf("Parsing completed successfully!\n");
    return 0;
}
