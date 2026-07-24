#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

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

    fprintf(stdout, "Here is the file: \n%s", source);

    Lexer lexer;
    lexer_init(&lexer, source);
    print_lex(&lexer);

    return 0;
}
