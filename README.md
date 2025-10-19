> **Grammer For The Project:**
>
1. Program Structure
Program        → FuncDecl*

2. Function Declarations
FuncDecl       → Type T_IDENTIFIER T_LPAREN Params T_RPAREN Block
Type           → T_INT | T_FLOAT | T_DOUBLE | T_STRING | T_BOOL | T_VOID
Params         → Param (T_COMMA Param)* | ε
Param          → Type T_IDENTIFIER

3. Blocks & Statements
Block          → T_LBRACE Stmt* T_RBRACE

Stmt           → ExprStmt
               | ReturnStmt
               | IfStmt
               | WhileStmt
               | ForStmt
               | Block
               | VarDeclStmt
               | EmptyStmt

ExprStmt       → Expr T_SEMICOLON
ReturnStmt     → T_RETURN Expr T_SEMICOLON
IfStmt         → T_IF T_LPAREN Expr T_RPAREN Stmt (T_ELSE Stmt)?
WhileStmt      → T_WHILE T_LPAREN Expr T_RPAREN Stmt
ForStmt        → T_FOR T_LPAREN ExprStmt ExprStmt Expr? T_RPAREN Stmt
VarDeclStmt    → Type T_IDENTIFIER (T_ASSIGN Expr)? T_SEMICOLON
EmptyStmt      → T_SEMICOLON

4. Expressions
Expr           → Assignment

Assignment     → T_IDENTIFIER T_ASSIGN Assignment    | OrExpr

OrExpr         → AndExpr (T_OR AndExpr)*
AndExpr        → Equality (T_AND Equality)*
Equality       → Relational ((T_EQ | T_NEQ) Relational)*
Relational     → AddExpr ((T_LT | T_GT | T_LEQ | T_GEQ) AddExpr)*
AddExpr        → MulExpr ((T_PLUS | T_MINUS) MulExpr)*
MulExpr        → Unary ((T_MULT | T_DIV | T_MOD) Unary)*
Unary          → (T_NOT | T_MINUS | T_PLUS | T_INC | T_DEC) Unary
               | Postfix
Postfix        → Primary ( (T_LPAREN ArgList? T_RPAREN) | (T_INC | T_DEC) )*
Primary        → T_IDENTIFIER
               | T_NUMBER
               | T_FLOATLIT
               | T_STRING_LITERAL
               | T_CHAR_LITERAL
               | T_TRUE
               | T_FALSE
               | T_LPAREN Expr T_RPAREN
ArgList        → Expr (T_COMMA Expr)*




