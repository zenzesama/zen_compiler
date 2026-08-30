#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

static char *read_file(const char *path) {
    FILE *fp = fopen(path, "rb");

    if (!fp) {
        fprintf(stderr, "Could not open file.\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buff = malloc(size + 1);
    fread(buff, 1, size, fp);
    buff[size] = '\0';

    fclose(fp);
    return buff;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Please give file to compile.\n");
        return 1;
    }
    
    char *source = read_file(argv[1]);

    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    Node *ast = parser_start(&parser);

    FILE *out = fopen("out.ll", "w");
    if (!out) {
        fprintf(stderr, "Could not open out.ll for writing.\n");
        return 1;
    }

    codegen_start(out, ast);
    fclose(out);

    free_node(ast);
    free(source);

    fprintf(stdout, "Wrote: out.ll\n");
    fprintf(stdout, "Run `clang out.ll -o program`\n");

    return 0;
}
