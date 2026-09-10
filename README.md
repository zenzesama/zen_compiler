# zen

Compiler for a toy language called `zen`, written in C, targeting LLVM IR.
<br>
Currently handles arithmetic expressions: `+ - * / %`, unary `-`, parens.

## Run
    make zenc
    ./zenc sample.zen
    clang out.ll -o program
    ./program


## Files
    lexer.c/h    source text -> tokens
    parser.c/h   tokens -> AST
    ast.c/h      tree node type
    codegen.c/h  AST -> LLVM IR
    main.c       wires the above together, reads the file, writes out.ll


## Language Grammar
    expression -> term ( ("+" | "-") term )*
    term       -> factor ( ("*" | "/" | "%") factor )*
    factor     -> "-" factor | primary
    primary    -> NUMBER | "(" expression ")"
