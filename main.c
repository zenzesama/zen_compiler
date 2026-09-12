#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"
#include "debug.h"

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
    int lexerTest = 0, prgmTest = 0;
    int opt;
    
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd':
                lexerTest = 1;
                prgmTest = 1;
                break;
            default:
                fprintf(stderr, "Please provide a file and flags.\n");
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Please provide a file to compile.\n");
        return 1;
    }
    
    char *source = read_file(argv[optind]);

    Lexer lexer;
    lexer_init(&lexer, source);

    if (lexerTest) {
        Lexer dbgLex;
        lexer_init(&dbgLex, source);
        lexer_test(&dbgLex);
    }

    Parser parser;
    parser_init(&parser, &lexer);

    Program program;
    parser_start(&parser, &program);
    
    if (prgmTest) {
        program_test(&program);
    }
/*
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
*/
    return 0;
}
