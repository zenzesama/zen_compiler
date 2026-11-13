# zen

Compiler for a toy language called `zen`, written in C, targeting LLVM IR.
<br>
Handles integer variables (`var int a = 2;`), arithmetic `+ - * / %`, unary `-` and `!`, comparisons `== != < <= > >=`, logic `&& ||`, `if`/`else`/`while` with `{ }` blocks, functions with recursion (`func f(int x) int`), and `print(expr);`. Use the `-d` flag for debug mode (token dump and AST).

## Example

    func fact(int n) int {
        if (n <= 1) {
            return 1;
        }
        return n * fact(n - 1);
    }

    func add(int a, int b) int {
        return a + b;
    }

    var int i = 1;
    var int sum = 0;
    while (i <= 4) {
        sum = add(sum, fact(i));
        i = i + 1;
    }
    print(sum);
    print(fact(5));

Output:

    33
    120


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
    - requires clang to work with the generated out.ll file
    - int is the only type. Zero is false and nonzero is true
    - top-level statements run as an implicit main; a top-level `return n;` sets the exit code
    - functions can't see top-level variables; max 8 parameters per function and 8 arguments per call
    - && and || evaluate both sides (no short-circuit)
    - variables are scoped per function, not per block
    - no comments

## Language Grammar
    program    -> ( funcDecl | statement )* EOF
    funcDecl   -> "func" IDENTIFIER "(" params? ")" type block
    params     -> type IDENTIFIER ( "," type IDENTIFIER )*
    statement  -> varDecl | assignStmt | exprStmt | printStmt | returnStmt
                  | ifStmt | whileStmt | block
    varDecl    -> "var" type IDENTIFIER ( "=" expression )? ";"
    assignStmt -> IDENTIFIER "=" expression ";"
    exprStmt   -> expression ";"
    printStmt  -> "print" "(" expression ")" ";"
    returnStmt -> "return" expression ";"
    ifStmt     -> "if" "(" expression ")" block ( "else" ( ifStmt | block ) )?
    whileStmt  -> "while" "(" expression ")" block
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
    primary    -> NUMBER | IDENTIFIER | IDENTIFIER "(" args? ")" | "(" expression ")"
    args       -> expression ( "," expression )*
