#include "Parser.h"

token Parser::peek() // look at the current token without consuming it
{
    if (current >= tokens.size())
        return token("EOF", "", -1);
    return tokens[current];
}

token Parser::advance() // consume the current token and return it
{
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::check(const string& type) // check if the current token matches the expected type
{
    if (isAtEnd()) return false;
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



Expr* Parser::primary() // literals and identifiers
{
    if (match({ "T_CHAR_LIT" })) 
        return new LiteralExpr(tokens[current - 1].val, "char");
    if (match({ "T_FLOAT_LIT" })) 
        return new LiteralExpr(tokens[current - 1].val, "float");
    if (match({ "T_STRING_LIT" })) 
        return new LiteralExpr(tokens[current - 1].val, "string");
    if (match({ "T_IDENTIFIER" })) 
        return new IdentifierExpr(tokens[current - 1].val);

    throw ParseError::ExpectedExpr();
}

Expr* Parser::factor()  // handles * and / operations
{
    Expr* expr = primary();
    while (match({ "T_MULT", "T_DIV" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = primary();
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

Expr* Parser::equality()  // handles == and != operations
{
    Expr* expr = term();
    while (match({ "T_EQ", "T_NEQ" })) 
    {
        string op = tokens[current - 1].val;
        Expr* right = term();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::expression()  // checks for equality first
{
    return equality();
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

vector<string> Parser::paramaeters() //function parameters
{
    vector<string> params;

    // Parse parameter list
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

	vector<string> params = paramaeters();

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
    string type = tokens[current - 1].type;

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

    return varDeclaration();
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
