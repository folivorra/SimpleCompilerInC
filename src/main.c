// main.c
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "generator.h"

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

    // Здесь можно добавить освобождение памяти AST и таблицы символов, если необходимо

    return 0;
}
