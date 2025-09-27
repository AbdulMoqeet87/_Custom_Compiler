#include "Parser.h"

token Parser::peek() // look at the current token without consuming it
{
    if (current >= tokens.size())
        return token("EOF", "", -1);
    return tokens[current];
}

token Parser::advance() // consume the current token and return it
{
    if (!isAtEnd()) 
        current++;
    return tokens[current - 1];
}

bool Parser::check(const string& type) // check if the current token matches the expected type
{
    if (isAtEnd()) 
        return false;
    return peek().type == type;
}

bool Parser::match(const vector<string>& types) // if the current token matches any in types, consume it and return true
{
    for (auto& t : types)
    {
        if (check(t))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::isAtEnd() // check if we've reached the end of the token list
{
    return current >= tokens.size();
}

Expr* Parser::primary() // literals, identifiers, and ( expression )
{
    if (match({ "T_IDENTIFIER" })) 
        return new IdentifierExpr(tokens[current - 1].val);

    // literal
    if (match({ "T_NUMBER" }))
        return new LiteralExpr(tokens[current - 1].val, "int");
    if (match({ "T_FLOAT_LIT" }))
        return new LiteralExpr(tokens[current - 1].val, "float");
    if (match({ "T_STRING_LIT" }))
        return new LiteralExpr(tokens[current - 1].val, "string");
    if (match({ "T_CHAR_LIT" }))
        return new LiteralExpr(tokens[current - 1].val, "char");

    if (match({"T_LBRACE"}))
    {
        current--;
        Expr* expr= expression();

        if(!match({"T_RBRACE"}))
			throw ParseError::UnexpectedToken(peek());

		return expr;
	}

    throw ParseError::ExpectedExpr();
}

vector<Expr*> Parser::arguments()
{
    vector<Expr*> args;
    if (!check("T_RPAREN"))
    {
        do
        {
            args.push_back(expression());
        } while (match({ "T_COMMA" }));
    }
	return args;
}

Expr* Parser::call()
{
    // call ? primary ( "(" arguments? ")" )* ;
	Expr* retType = primary();

    if(match({"T_LPAREN"}))
    {
        vector<Expr*> args = arguments();
        if (!match({ "T_RPAREN" }))
            throw ParseError::UnexpectedToken(peek());

		return new callExpr(dynamic_cast<IdentifierExpr*>(retType)->name, args);
    }
}

Expr* Parser::unary() // handles unary operations like - and !
{
    if (match({ "T_NOT" }))
    {
        string op = tokens[current - 1].val;
        Expr* right = unary();
        return new BinaryExpr(new LiteralExpr("0", "int"), op, right); // e.g., -a becomes 0 - a
    }
    return call();
}

Expr* Parser::factor()  // handles * and / operations
{
    Expr* expr = unary();
    while (match({ "T_MULT", "T_DIV" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = unary();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::term()  // handles + and - operations
{
	Expr* expr = factor(); // get the left operand
    while (match({ "T_PLUS", "T_MINUS" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = factor();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::comparison()  // handles <, >, <=, >= operations
{
    // comparison    ? term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
    Expr* expr = term();
    while (match({ "T_LT", "T_LEQ", "T_GT", "T_GEQ" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = term();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::equality()  // handles == and != operations
{
	Expr* expr = comparison();

    while (match({ "T_EQ", "T_NEQ" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = comparison();
        expr = new BinaryExpr(expr, op, right);
	}
	return expr;
}

Expr* Parser::logicalAnd()
{
    Expr* expr = equality();
    while (match({ "T_AND" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = equality();
        expr = new BinaryExpr(expr, op, right);
    }
	return expr;
}

Expr* Parser::logicalOr()
{
    Expr* expr = logicalAnd();
    while (match({ "T_OR" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = logicalAnd();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::assignment() // handles assignment operations
{
    
    if (match({ "T_IDENTIFIER" }))
    {
        Expr* left = nullptr;
        left = new IdentifierExpr(tokens[current - 1].val);

        if (match({ "T_ASSIGN" }))
        {
            string op = tokens[current - 1].val;
            Expr* right = assignment();
            if (IdentifierExpr* id = dynamic_cast<IdentifierExpr*>(left))
            {
                return new BinaryExpr(left, op, right);
            }
			throw ParseError::ExpectedExpr();
        }
        
    }

	return logicalOr();
}

Expr* Parser::expression()  // checks for equality first
{
	return assignment();
}

Stmt* Parser::exprStatement()
{
    Expr* expr = expression();
    if (!match({ "T_SEMICOLON" }))
        throw ParseError::UnexpectedToken(peek());
    return new ExprStmt(expr);
}

Block* Parser::block() 
{
    if (!match({ "T_LBRACE" })) 
    {
        throw ParseError::ExpectedTypeToken();
    }
    Block* block = new Block();
    while (!check("T_RBRACE") && !isAtEnd()) 
    {
        block->statements.push_back(declaration());
    }
    if (!match({ "T_RBRACE" })) 
    {
        throw ParseError::ExpectedTypeToken();
    }
    return block;
}

vector<string> Parser::parameters() //function parameters
{
    vector<string> params;

    if (!check("T_RPAREN"))
    {
        do
        {
            if (!match({ "T_IDENTIFIER" }))
                throw ParseError::ExpectedIdentifier();
            params.push_back(tokens[current - 1].val);
        } while (match({ "T_COMMA" }));
    }
    if (!match({ "T_RPAREN" }))
		throw ParseError::ExpectedTypeToken(); // Expect ')'
	return params;
}

Stmt* Parser::funcDeclaration() 
{
    if (!match({ "T_FUNCTION" })) 
    {
        throw ParseError::UnexpectedToken(peek());
    }
    if (!match({ "T_IDENTIFIER" })) 
    {
        throw ParseError::ExpectedIdentifier();
    }

    string name = tokens[current - 1].val;

    if (!match({ "T_LPAREN" }))
        throw ParseError::ExpectedTypeToken(); // Expect '('

	vector<string> params = parameters();

    Block* body = block();
    return new funcDecl(name, params, body);
}

vector<string> Parser::type()
{
    vector<string> types;
    if (match({ "T_INT", "T_FLOAT", "T_STRING", "T_BOOL", "T_DOUBLE", "T_LONG", "T_SHORT" })) //types
    {
        types.push_back(tokens[current - 1].type);
    }
    else
    {
        throw ParseError::ExpectedTypeToken();
    }
	return types;
}


Stmt* Parser::varDeclaration() 
{
    if (!match(type())) //types
    {
        throw ParseError::ExpectedTypeToken();
    }
    string type = tokens[current - 1].val;

    if (!match({ "T_IDENTIFIER" })) 
    {
        throw ParseError::ExpectedIdentifier();
    }
    string name = tokens[current - 1].val;

    Expr* initializer = nullptr;
    if (match({ "T_ASSIGN" })) 
    {
        initializer = expression();
    }

    if (!match({ "T_SEMICOLON" })) 
    {
        throw ParseError::UnexpectedToken(peek());
    }

    return new VarDecl(type, name, initializer);
}


Stmt* Parser::declaration()
{
    // extend later for if, while, etc.

    if (match({ "T_FUNCTION" }))
    {
        current--; // step back to let funcDeclaration handle the 'function' token
        return funcDeclaration();
    }

	return varDeclaration(); // default to variable declaration
}

Stmt* Parser::ifStatement() 
{
    if(match({ "T_IF" }))
    {
        if (!match({ "T_LPAREN" }))
            throw ParseError::UnexpectedToken(peek());
        Expr* condition = expression();
        if (!match({ "T_RPAREN" }))
            throw ParseError::UnexpectedToken(peek());
        Stmt* thenBranch = statement();
        Stmt* elseBranch = nullptr;
        if (match({ "T_ELSE" })) 
        {
            elseBranch = statement();
        }
        return new IfStmt(condition, thenBranch, elseBranch);
	}
	throw ParseError::UnexpectedToken(peek());
}

Stmt* Parser::whileStatement() 
{
    if (match({ "T_LPAREN" }))
    {
        Expr* condition = expression();
        if (!match({ "T_RPAREN" }))
            throw ParseError::UnexpectedToken(peek());
		if (match({ "T_SEMICOLON" }))
			throw ParseError::UnexpectedToken(peek());
        if (match({ "T_RBRACE" }))
        {
            Stmt* body = statement();
            if(!match({ "T_RBRACE" }))
				throw ParseError::UnexpectedToken(peek());
            return new WhileStmt(condition, body);
        }
    }
	throw ParseError::UnexpectedToken(peek());
}

Stmt* Parser::returnStatement() 
{
	Expr* expr = nullptr;
    if (match({ "T_SEMICOLON" })) // 
        return new ReturnStmt(expr); // void return
    expr = expression();
    if (!match({ "T_SEMICOLON" }))
        throw ParseError::UnexpectedToken(peek());
	return new ReturnStmt(expr);
}

Stmt* Parser::forStatement() 
{
    if (match({ "T_LPAREN" }))
    {
        Stmt* init = declaration(); // initialization
        if(!match({ "T_SEMICOLON" }))
            throw ParseError::ExpectedForLoopParts();

        Expr* condition = expression(); // condition
        if (!match({ "T_SEMICOLON" }))
            throw ParseError::ExpectedForLoopParts();

        Expr* update = expression(); // increment

        if (!match({ "T_RPAREN" }))
            throw ParseError::ExpectedForLoopParts();

        Stmt* body = nullptr;

        if (match({ "T_LBRACE" }))
        {
            body = statement(); // loop body

            if (!match({ "T_RBRACE" }))
                throw ParseError::UnexpectedToken(peek());
			return new ForStmt(init, condition, update, body);
        }
        else 
        {
            body = statement(); // loop body
            return new ForStmt(init, condition, update, body);
        }

		throw ParseError::UnexpectedToken(peek());
        
    }
}

Stmt* Parser::breakStatement()
{
    if (!match({ "T_SEMICOLON" }))
        throw ParseError::UnexpectedToken(peek());
    return new BreakStmt();
}

Stmt* Parser::switchStatement()
{
    if(match({ "T_LPAREN" }))
    {
        Expr* expr = expression();
        if (!match({ "T_RPAREN" }))
            throw ParseError::UnexpectedToken(peek());

        if (!match({ "T_LBRACE" }))
            throw ParseError::UnexpectedToken(peek());

        vector<pair<Expr*, Stmt*>> cases;
        Stmt* defaultCase = nullptr;

        while (!check("T_RBRACE") && !isAtEnd())
        {
            if (match({ "T_CASE" }))
            {
                Expr* caseExpr = expression();
                if (!match({ "T_COLON" }))
                    throw ParseError::UnexpectedToken(peek());
                Stmt* caseStmt = statement();
                cases.push_back({ caseExpr, caseStmt });
                if(match({ "T_BREAK" }))
                {
                    if (!match({ "T_SEMICOLON" }))
                        throw ParseError::UnexpectedToken(peek());
				}
                else if (match({ "T_CONTINUE" }))
                {
                    if (!match({ "T_SEMICOLON" }))
                        throw ParseError::UnexpectedToken(peek());
                }
            }
            else if (match({ "T_DEFAULT" }))
            {
                if (!match({ "T_COLON" }))
                    throw ParseError::UnexpectedToken(peek());
                defaultCase = statement();
                if (match({ "T_BREAK" }))
                {
                    if (!match({ "T_SEMICOLON" }))
                        throw ParseError::UnexpectedToken(peek());
                }
                else if (match({ "T_CONTINUE" }))
                {
                    if (!match({ "T_SEMICOLON" }))
                        throw ParseError::UnexpectedToken(peek());
				}
            }
            else
            {
                throw ParseError::UnexpectedToken(peek());
            }
        }
        if (!match({ "T_RBRACE" }))
            throw ParseError::UnexpectedToken(peek());

        return new SwitchStmt(expr, cases, defaultCase);
	}

	throw ParseError::UnexpectedToken(peek());
}

Stmt* Parser::statement() 
{
    if (match({ "T_IF" })) 
        return ifStatement();
    if (match({ "T_FOR" }))
        return forStatement();
    if (match({ "T_WHILE" })) 
        return whileStatement();
    if (match({ "T_SWITCH" }))
        return switchStatement();
    if (match({ "T_BREAK" }))
        return breakStatement();
    if (match({ "T_RETURN" })) 
        return returnStatement();
    if (match({"T_CONTINUE"}))
		return breakStatement();
    if (match({ "T_LBRACE" })) 
        return block();

    return exprStatement();
}

Program* Parser::parseProgram()  // parse the complete source code
{
    Program* program = new Program();
    while (!isAtEnd()) 
    {
        program->statements.push_back(declaration());
    }
    return program;
}

// add function calls, assignments,if-else, loops, return statements, unary expressions, paranthesis expression etc. later