//#include "Parser.h"
//
//token Parser::peek()
//{
//    if (current >= tokens.size())
//        return token("EOF", "", -1);
//    return tokens[current];
//}
//
//token Parser::advance()
//{
//    if (!isAtEnd())
//        current++;
//    return tokens[current - 1];
//}
//
//bool Parser::check(const string& type)
//{
//    if (isAtEnd())
//        return false;
//    return peek().type == type;
//}
//
//bool Parser::match(const vector<string>& types)
//{
//    for (auto& t : types)
//    {
//        if (check(t))
//        {
//            advance();
//            return true;
//        }
//    }
//    return false;
//}
//
//bool Parser::isAtEnd()
//{
//    return current >= tokens.size();
//}
//
//Expr* Parser::primary()
//{
//    if (match({ "T_IDENTIFIER" }))
//        return new IdentifierExpr(tokens[current - 1].val);
//
//    // Literals
//    if (match({ "T_NUMBER" }))
//        return new LiteralExpr(tokens[current - 1].val, "int");
//    if (match({ "T_FLOAT_LIT" }))
//        return new LiteralExpr(tokens[current - 1].val, "float");
//    if (match({ "T_STRING_LIT" }))
//        return new LiteralExpr(tokens[current - 1].val, "string");
//    if (match({ "T_CHAR_LIT" }))
//        return new LiteralExpr(tokens[current - 1].val, "char");
//    if (match({ "T_TRUE", "T_FALSE" }))
//        return new LiteralExpr(tokens[current - 1].val, "bool");
//
//    // Parentheses
//    if (match({ "T_LPAREN" }))
//    {
//        Expr* expr = expression();
//        if (!match({ "T_RPAREN" }))
//            throw ParseError::UnexpectedToken(peek());
//        return expr;
//    }
//
//    throw ParseError::ExpectedExpr();
//}
//
//vector<Expr*> Parser::arguments()
//{
//    vector<Expr*> args;
//    if (!check("T_RPAREN"))
//    {
//        do
//        {
//            args.push_back(expression());
//        } while (match({ "T_COMMA" }));
//    }
//    return args;
//}
//
//Expr* Parser::call()
//{
//    Expr* expr = primary();
//
//    // Handle function calls and method calls
//    while (match({ "T_LPAREN" }))
//    {
//        vector<Expr*> args = arguments();
//        if (!match({ "T_RPAREN" }))
//            throw ParseError::UnexpectedToken(peek());
//
//        if (IdentifierExpr* id = dynamic_cast<IdentifierExpr*>(expr)) {
//            expr = new callExpr(id->name, args);
//        }
//        // You might want to handle other cases like method calls: obj.func()
//        else {
//            throw ParseError::ExpectedIdentifier();
//        }
//    }
//    return expr;
//}
//
//Expr* Parser::unary()
//{
//    if (match({ "T_NOT", "T_MINUS", "T_MOF"}))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = unary();
//        return new UnaryExpr(op, right);
//    }
//    return call();
//}
//
//Expr* Parser::factor()
//{
//    Expr* expr = unary();
//    while (match({ "T_MULT", "T_DIV", "T_MOD"}))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = unary();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::term()
//{
//    Expr* expr = factor();
//    while (match({ "T_PLUS", "T_MINUS" }))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = factor();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::comparison()
//{
//    Expr* expr = term();
//    while (match({ "T_LT", "T_LEQ", "T_GT", "T_GEQ" }))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = term();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::equality()
//{
//    Expr* expr = comparison();
//    while (match({ "T_EQ", "T_NEQ" }))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = comparison();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::logicalAnd()
//{
//    Expr* expr = equality();
//    while (match({ "T_AND" }))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = equality();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::logicalOr()
//{
//    Expr* expr = logicalAnd();
//    while (match({ "T_OR" }))
//    {
//        string op = tokens[current - 1].val;
//        Expr* right = logicalAnd();
//        expr = new BinaryExpr(expr, op, right);
//    }
//    return expr;
//}
//
//Expr* Parser::assignment()
//{
//    if (match({ "T_IDENTIFIER" }))
//    {
//        Expr* left = new IdentifierExpr(tokens[current - 1].val);
//
//        if (match({ "T_ASSIGN" }))
//        {
//            string op = tokens[current - 1].val;
//            Expr* right = assignment();
//            if (IdentifierExpr* id = dynamic_cast<IdentifierExpr*>(left))
//            {
//                return new BinaryExpr(left, op, right);
//            }
//            throw ParseError::ExpectedExpr();
//        }
//        else
//        {
//            // Not an assignment - continue with logicalOr to handle operators like +
//            current--; // Put the identifier back so logicalOr can handle it
//            return logicalOr();
//        }
//    }
//    return logicalOr();
//}
//
//Expr* Parser::expression()
//{
//    return assignment();
//}
//
//Stmt* Parser::exprStatement()
//{
//    Expr* expr = expression();
//    if (!match({ "T_SEMICOLON" }))
//        throw ParseError::UnexpectedToken(peek());
//    return new ExprStmt(expr);
//}
//
//Block* Parser::block()
//{
//    if (!match({ "T_LBRACE" }))
//    {
//        throw ParseError::ExpectedTypeToken();
//    }
//    Block* block = new Block();
//    while (!check("T_RBRACE") && !isAtEnd())
//    {
//        block->statements.push_back(statement());
//    }
//    if (!match({ "T_RBRACE" }))
//    {
//        throw ParseError::ExpectedTypeToken();
//    }
//    return block;
//}
//
//vector<string> Parser::parameters()
//{
//    vector<string> params;
//
//    if (!check("T_RPAREN"))
//    {
//        do
//        {
//            // Parse parameter type
//            string paramType = parseReturnType(); // Reuse the same logic
//
//            if (!match({ "T_IDENTIFIER" }))
//                throw ParseError::ExpectedIdentifier();
//
//            // Store as "type name" or just name depending on your design
//            string paramName = tokens[current - 1].val;
//            params.push_back(paramType + " " + paramName); // Or just paramName if you want to store types separately
//
//        } while (match({ "T_COMMA" }));
//    }
//    if (!match({ "T_RPAREN" }))
//        throw ParseError::ExpectedTypeToken();
//    return params;
//}
//
//string Parser::parseReturnType()
//{
//    // Handle void return type
//    if (match({ "T_VOID" })) {
//        return "void";
//    }
//
//    // Handle other types
//    string type;
//    if (match({ "T_INT" }))
//        type = "int";
//    else if (match({ "T_FLOAT" }))
//        type = "float";
//    else if (match({ "T_STRING" }))
//        type = "string";
//    else if (match({ "T_BOOL" }))
//        type = "bool";
//    else if (match({ "T_DOUBLE" }))
//        type = "double";
//    else if (match({ "T_LONG" }))
//        type = "long";
//    else if (match({ "T_SHORT" }))
//        type = "short";
//    else if (match({ "T_CHAR" }))
//        type = "char";
//    else
//        throw ParseError::ExpectedTypeToken();
//
//    return type;
//}
//
//
//Stmt* Parser::varDeclaration()
//{
//    string type;
//    if (match({ "T_INT" }))
//        type = "int";
//    else if (match({ "T_FLOAT" }))
//        type = "float";
//    else if (match({ "T_STRING" }))
//        type = "string";
//    else if (match({ "T_BOOL" }))
//        type = "bool";
//    else if (match({ "T_DOUBLE" }))
//        type = "double";
//    else if (match({ "T_LONG" }))
//        type = "long";
//    else if (match({ "T_SHORT" }))
//        type = "short";
//    else
//        throw ParseError::ExpectedTypeToken();
//
//    if (!match({ "T_IDENTIFIER" }))
//    {
//        throw ParseError::ExpectedIdentifier();
//    }
//    string name = tokens[current - 1].val;
//
//    Expr* initializer = nullptr;
//    if (match({ "T_ASSIGN" }))
//    {
//        initializer = expression();
//    }
//
//    if (!match({ "T_SEMICOLON" }))
//    {
//        throw ParseError::UnexpectedToken(peek());
//    }
//
//    return new VarDecl(type, name, initializer);
//}
//
//Stmt* Parser::declaration()
//{
//    // Only try function parsing if we start with a valid type
//    if (check("T_INT") || check("T_FLOAT") || check("T_VOID") || check("T_STRING") ||
//        check("T_BOOL") || check("T_DOUBLE") || check("T_LONG") || check("T_SHORT") || check("T_CHAR")) {
//
//        size_t saved = current;
//
//        try {
//            string returnType = parseReturnType();
//
//            // Peek ahead without consuming to check function pattern
//            size_t peekPos = current;
//            if (check("T_IDENTIFIER")) {
//                string potentialName = peek().val;
//                peekPos++;
//                // Look ahead one more token to see if it's '('
//                if (peekPos < tokens.size() && tokens[peekPos].type == "T_LPAREN") {
//                    // Definitely a function - consume the tokens
//                    match({ "T_IDENTIFIER" }); // Consume the name
//                    string name = tokens[current - 1].val;
//
//                    if (!match({ "T_LPAREN" }))
//                        throw ParseError::ExpectedTypeToken();
//
//                    vector<string> params = parameters();
//                    Block* body = nullptr;
//
//                    // Function definition with body
//                    if (check("T_LBRACE")) {
//                        // Function definition: has body
//                        body = block();
//                    }
//                    else if (match({ "T_SEMICOLON" })) {
//                        // Function declaration ends with semicolon, no body
//                        body = nullptr;
//                    }
//                    else {
//                        throw ParseError::UnexpectedToken(peek());
//                    }
//
//                    return new funcDecl(returnType, name, params, body);
//                }
//            }
//            // Not a function - restore
//            current = saved;
//        }
//        catch (const ParseError&) {
//            current = saved;
//        }
//    }
//
//    return varDeclaration();
//}
//
//Stmt* Parser::continueStatement()
//{
//    if (!match({ "T_SEMICOLON" }))
//        throw ParseError::UnexpectedToken(peek());
//    return new ContinueStmt();
//}
//
//Stmt* Parser::ifStatement()
//{
//    if (!match({ "T_LPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//    Expr* condition = expression();
//    if (!match({ "T_RPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//	// if left curly brakets then it's a block statement
//    Stmt* thenBranch = block();
//    Stmt* elseBranch = nullptr;
//    if (match({ "T_ELSE" }))
//    {
//		// if again if, then it's an else if statement
//        if(match({ "T_IF" }))
//        {
//            elseBranch = ifStatement();
//		}
//        else
//        {
//			elseBranch = block();
//        }
//    }
//    return new IfStmt(condition, thenBranch, elseBranch);
//}
//
//Stmt* Parser::whileStatement()
//{
//    if (!match({ "T_LPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//    Expr* condition = expression();
//    if (!match({ "T_RPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//    // Allow either a block or a single statement as the loop body
//	Stmt* body = block();
//    return new WhileStmt(condition, body);
//}
//
//Stmt* Parser::returnStatement()
//{
//    Expr* expr = nullptr;
//    if (!check("T_SEMICOLON")) {
//        expr = expression();
//    }
//    if (!match({ "T_SEMICOLON" }))
//        throw ParseError::UnexpectedToken(peek());
//    return new ReturnStmt(expr);
//}
//
//Stmt* Parser::forStatement()
//{
//    
//    if (!match({ "T_LPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//
//    Stmt* init = nullptr;
//    if (!check("T_SEMICOLON")) {
//        // Check if it's a variable declaration or expression
//        if (check("T_INT") || check("T_FLOAT") || check("T_STRING") ||
//            check("T_BOOL") || check("T_DOUBLE") || check("T_LONG") ||
//            check("T_SHORT") || check("T_CHAR")) {
//            init = varDeclaration();  // Variable declaration: int i = 0
//        }
//        else 
//        {
//            init = exprStatement();   // Expression: int i = 0, x = 5, etc.
//        }
//    }
//
//    Stmt* condition = exprStatement();
//
//	Expr* update = expression();
//
//    if (!match({ "T_RPAREN" }))
//		throw ParseError::UnexpectedToken(peek());
//
//	Stmt* body = block();
//    return new ForStmt(init, condition, update, body);
//    
//}
//
//Stmt* Parser::breakStatement()
//{
//    if (!match({ "T_SEMICOLON" }))
//        throw ParseError::UnexpectedToken(peek());
//    return new BreakStmt();
//}
//
//Stmt* Parser::switchStatement()
//{
//    if (!match({ "T_LPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//    Expr* expr = expression();
//    if (!match({ "T_RPAREN" }))
//        throw ParseError::UnexpectedToken(peek());
//
//    if (!match({ "T_LBRACE" }))
//        throw ParseError::UnexpectedToken(peek());
//
//    vector<pair<Expr*, Stmt*>> cases;
//    Stmt* defaultCase = nullptr;
//
//    while (!check("T_RBRACE") && !isAtEnd())
//    {
//        if (match({ "T_CASE" }))
//        {
//            Expr* caseExpr = expression();
//            if (!match({ "T_COLON" }))
//                throw ParseError::UnexpectedToken(peek());
//            Stmt* caseStmt = statement();
//            cases.push_back({ caseExpr, caseStmt });
//        }
//        else if (match({ "T_DEFAULT" }))
//        {
//            if (!match({ "T_COLON" }))
//                throw ParseError::UnexpectedToken(peek());
//            defaultCase = statement();
//        }
//        else
//        {
//            throw ParseError::UnexpectedToken(peek());
//        }
//    }
//    if (!match({ "T_RBRACE" }))
//        throw ParseError::UnexpectedToken(peek());
//
//    return new SwitchStmt(expr, cases, defaultCase);
//}
//
//Stmt* Parser::statement()
//{
//    if (match({ "T_IF" }))
//        return ifStatement();
//    if (match({ "T_FOR" }))
//        return forStatement();
//    if (match({ "T_WHILE" }))
//        return whileStatement();
//    if (match({ "T_SWITCH" }))
//        return switchStatement();
//    if (match({ "T_BREAK" }))
//        return breakStatement();
//    if (match({ "T_CONTINUE" }))
//        return continueStatement();
//    if (match({ "T_RETURN" }))
//        return returnStatement();
//
//    if (match({ "T_LBRACE" }))
//        return block();
//
//    if (check("T_INT") || check("T_FLOAT") || check("T_STRING") ||
//        check("T_BOOL") || check("T_DOUBLE") || check("T_LONG") ||
//        check("T_SHORT") || check("T_CHAR")) {
//        return varDeclaration();
//    }
//
//    return exprStatement();
//}
//
//Program* Parser::parseProgram()
//{
//    Program* program = new Program();
//    while (!isAtEnd())
//    {
//        program->statements.push_back(declaration());
//    }
//    return program;
//}