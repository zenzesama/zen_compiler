# zen

Compiler for a toy language called `zen`, written in C, targeting LLVM IR.
<br>
Currently handles arithmetic expressions: `+ - * / %`, unary `-`, parens and integer variables that can be declared with `var int a = 2;`. Use `-d` flag for debug mode.

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
    debug.c/.h   optional debug mode for compiler, reads out the tokens and parsed peogram.
    main.c       wires the above together, reads the file, writes out.ll


## Language Grammar
    program    -> statement* EOF
    statement  -> varDecl | exprStmt
    varDecl    -> "var" type IDENTIFIER ( "=" expression )? ";"
    exprStmt   -> expression ";"
    type       -> "int"
    expression -> assignment
    assignment -> IDENTIFIER "=" assignment | term
    term       -> factor ( ( "+" | "-" ) factor )*
    factor     -> unary ( ( "*" | "/" | "%" ) unary )*
    unary      -> "-" unary | primary
    primary    -> NUMBER | IDENTIFIER | "(" expression ")"
