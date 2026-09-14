# zen

Compiler for a toy language called `zen`, written in C, targeting LLVM IR.
<br>
Currently handles arithmetic expressions: `+ - * / %`, unary `-`, parens, integer variables (`var int a = 2;`), and `print(expr);`. Use `-d` flag for debug mode.

## Example
    var int a = 2;
    var int b = 8;
    var int sum = a + b;
    print(sum);

    var int x;
    x = a * b - 2;
    print(x);

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
    debug.c/.h   optional debug mode for compiler, reads out the tokens and parsed program.
    main.c       wires the above together, reads the file, writes out.ll


## Language Grammar
    program    -> statement* EOF
    statement  -> varDecl | exprStmt | printStmt
    varDecl    -> "var" type IDENTIFIER ( "=" expression )? ";"
    exprStmt   -> expression ";"
    printStmt  -> "print" "(" expression ")" ";"
    type       -> "int"
    expression -> assignment
    assignment -> IDENTIFIER "=" assignment | term
    term       -> factor ( ( "+" | "-" ) factor )*
    factor     -> unary ( ( "*" | "/" | "%" ) unary )*
    unary      -> "-" unary | primary
    primary    -> NUMBER | IDENTIFIER | "(" expression ")"
