# zen

Compiler for a toy language called `zen`, written in C, targeting LLVM IR.
<br>
Handles integer variables (`var int a = 2;`), arithmetic `+ - * / %`, unary `-` and `!`, comparisons `== != < <= > >=`, logic `&& ||`, `if`/`else` with `{ }` blocks, and `print(expr);`. Use the `-d` flag for debug mode (token dump and AST).

## Example
    var int a = 7;
    var int b = 3;
    var int sum = a + b;
    print(sum);

    if (a > b && b != 0) {
        print(a);
    } else {
        print(b);
    }

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
    debug.c/.h   optional debug mode, prints the tokens and parsed program
    main.c       wires the above together, reads the file, writes out.ll

## Notes
    - int is the only type; nonzero is true
    - && and || evaluate both sides (no short-circuit)
    - variables are not block-scoped

## Language Grammar
    program    -> statement* EOF
    statement  -> varDecl | assignStmt | exprStmt | printStmt | ifStmt | block
    varDecl    -> "var" type IDENTIFIER ( "=" expression )? ";"
    assignStmt -> IDENTIFIER "=" expression ";"
    exprStmt   -> expression ";"
    printStmt  -> "print" "(" expression ")" ";"
    ifStmt     -> "if" "(" expression ")" block ( "else" ( ifStmt | block ) )?
    block      -> "{" statement* "}"
    type       -> "int"
    expression -> logic_or
    logic_or   -> logic_and ( "||" logic_and )*
    logic_and  -> equality ( "&&" equality )*
    equality   -> comparison ( ( "==" | "!=" ) comparison )*
    comparison -> term ( ( "<" | "<=" | ">" | ">=" ) term )*
    term       -> factor ( ( "+" | "-" ) factor )*
    factor     -> unary ( ( "*" | "/" | "%" ) unary )*
    unary      -> ( "-" | "!" ) unary | primary
    primary    -> NUMBER | IDENTIFIER | "(" expression ")"
