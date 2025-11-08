
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "with_regex_Lexer.h" 

using namespace std;

enum ParseError
{
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr,
};


struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
protected:
    static void printIndent(int n) 
    {
        for (int i = 0; i < n; ++i) 
            cout << "  ";
    }
};

using ASTPtr = shared_ptr<ASTNode>;

struct Expr : ASTNode
{
};
using ExprPtr = shared_ptr<Expr>;

struct Stmt : ASTNode
{
};
using StmtPtr = shared_ptr<Stmt>;


struct IdentifierExpr : Expr
{
    string name;
    IdentifierExpr(string n) : name(n) {}
    void print(int indent = 0) const override 
    {
        printIndent(indent);
        cout << "Identifier(" << name << ")\n";
    }
};

struct IntLiteral : Expr
{
    string val;
    IntLiteral(string v) : val(v) {}
    void print(int indent = 0) const override 
    {
        printIndent(indent);
        cout << "Int(" << val << ")\n";
    }
};

struct FloatLiteral : Expr
{
    string val;
    FloatLiteral(const string& v) : val(v) {}
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Float(" << val << ")\n";
    }
};

struct StringLiteral : Expr
{
    string val;
    StringLiteral(const string& v) : val(v) {}
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "String(" << val << ")\n";
    }
};

struct BoolLiteral : Expr
{
    string val;
    BoolLiteral(const string& v) : val(v) {}
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Bool(" << val << ")\n";
    }
};

struct CharLiteral : Expr
{
    string val;
    CharLiteral(const string& v) : val(v) {}
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Char(" << val << ")\n";
    }
};

struct UnaryExpr : Expr
{
    string op;
    ExprPtr rhs;
    UnaryExpr(const string& o, ExprPtr r) : op(o), rhs(r) {}
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "UnaryOp(" << op << ")\n";
        if (rhs) rhs->print(indent + 1);
    }
};

struct BinaryExpr : Expr
{
    string op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(ExprPtr l, const string& o, ExprPtr r) : left(l), op(o), right(r) {}
    void print(int indent = 0) const override
    {
        printIndent(indent);
        cout << "BinaryOp(" << op << ")\n";
        if (left) left->print(indent + 1);
        if (right) right->print(indent + 1);
    }
};


struct CallExpr : Expr
{
    ExprPtr callee;
    vector<ExprPtr> args;
    CallExpr(ExprPtr c) : callee(c) {}
    void print(int indent = 0) const override
    {
        printIndent(indent);
        cout << "Call\n";
        callee->print(indent + 1);
        printIndent(indent + 1);
        cout << "Args:\n";
        for (auto& a : args) a->print(indent + 2);
    }
};


struct PostfixExpr : Expr
{
    ExprPtr base;
    string op;
    PostfixExpr(ExprPtr b, const string& o) : base(b), op(o) {}
    void print(int indent = 0) const override
    {
        printIndent(indent);
        cout << "Postfix(" << op << ")\n";
        base->print(indent + 1);
    }
};

// Statements
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(e) {}
    void print(int indent = 0) const override {
        printIndent(indent); cout << "ExprStmt\n"; if (expr) expr->print(indent + 1);
    }
};

struct ReturnStmt : Stmt
{
    ExprPtr expr;
    ReturnStmt(ExprPtr e) : expr(e) {}
    void print(int indent = 0) const override {
        printIndent(indent); cout << "Return\n"; if (expr) expr->print(indent + 1);
    }
};

struct VarDeclStmt : Stmt
{
    string typeTok;
    string name;
    ExprPtr init;
    VarDeclStmt(const string& t, const string& n, ExprPtr i) : typeTok(t), name(n), init(i) {}
    void print(int indent = 0) const override {
        printIndent(indent); cout << "VarDecl (" << typeTok << " " << name << ")\n";
        if (init) { printIndent(indent + 1); cout << "Init:\n"; init->print(indent + 2); }
    }
};

struct IfStmt : Stmt
{
    ExprPtr cond;
    StmtPtr thenStmt;
    StmtPtr elseStmt;
    IfStmt(ExprPtr c, StmtPtr t, StmtPtr e = nullptr) : cond(c), thenStmt(t), elseStmt(e)
    {}
    void print(int indent = 0) const override
    {
        printIndent(indent); cout << "If\n";
        printIndent(indent + 1); cout << "Cond:\n"; if (cond) cond->print(indent + 2);
        printIndent(indent + 1); cout << "Then:\n"; if (thenStmt) thenStmt->print(indent + 2);
        if (elseStmt)
        {
            printIndent(indent + 1); cout << "Else:\n"; elseStmt->print(indent + 2);
        }
    }
};

struct WhileStmt : Stmt
{
    ExprPtr cond;
    StmtPtr body;
    WhileStmt(ExprPtr c, StmtPtr b) : cond(c), body(b)
    {}
    void print(int indent = 0) const override
    {
        printIndent(indent); cout << "While\n";
        printIndent(indent + 1); cout << "Cond:\n"; if (cond) cond->print(indent + 2);
        printIndent(indent + 1); cout << "Body:\n"; if (body) body->print(indent + 2);
    }
};

struct ForStmt : Stmt
{
    StmtPtr init;
    StmtPtr condStmt;
    ExprPtr iterExpr;
    StmtPtr body;
    ForStmt(StmtPtr i, StmtPtr c, ExprPtr it, StmtPtr b) : init(i), condStmt(c), iterExpr(it), body(b) {}
    void print(int indent = 0) const override
    {
        printIndent(indent); cout << "For\n";
        printIndent(indent + 1); cout << "Init:\n"; if (init) init->print(indent + 2);
        printIndent(indent + 1); cout << "CondStmt:\n"; if (condStmt) condStmt->print(indent + 2);
        if (iterExpr) { printIndent(indent + 1); cout << "Iter:\n"; iterExpr->print(indent + 2); }
        printIndent(indent + 1); cout << "Body:\n"; if (body) body->print(indent + 2);
    }
};

struct BlockStmt : Stmt
{
    vector<StmtPtr> stmts;
    void print(int indent = 0) const override
    {
        printIndent(indent); cout << "Block\n";
        for (auto& s : stmts) if (s) s->print(indent + 1);
    }
};

// Function declaration
struct Param
{
    string typeTok;
    string name;
};

struct FuncDecl : ASTNode
{
    string retType;
    string name;
    vector<Param> params;
    shared_ptr<BlockStmt> body;
    FuncDecl() = default;
    void print(int indent = 0) const override
    {
        printIndent(indent);
        cout << "FuncDecl " << name << " : " << retType << "\n";
        printIndent(indent + 1); cout << "Params:\n";
        for (auto& p : params)
        {
            printIndent(indent + 2);
            cout << p.typeTok << " " << p.name << "\n";
        }
        if (body)
        {
            printIndent(indent + 1); cout << "Body:\n";
            body->print(indent + 2);
        }
    }
};

//struct Program : ASTNode
//{
//    vector<shared_ptr<FuncDecl>> functions;
//    void print(int indent = 0) const override
//    {
//        cout << "Program\n";
//        for (auto& f : functions) f->print(indent + 1);
//    }
//};
struct Program : ASTNode
{
    vector<shared_ptr<FuncDecl>> functions;
    vector<shared_ptr<VarDeclStmt>> globals; // Added to store global variables

    void print(int indent = 0) const override
    {
        cout << "Program\n";

        if (!globals.empty()) {
            printIndent(indent + 1);
            cout << "Globals:\n";
            for (auto& g : globals)
                g->print(indent + 2);
        }

        for (auto& f : functions)
            f->print(indent + 1);
    }
};


class Parser
{
    vector<token> tokens;
    size_t pos;
    Lexer_regex lexer;

    token peekToken() const
    {
        if (pos < tokens.size()) return tokens[pos];
        return token("eof", "UnexpectedEOF", 0);
    }
    token peekNext() const
    {
        if (pos + 1 < tokens.size()) return tokens[pos + 1];
        return token("eof", "UnexpectedEOF", 0);
    }
    token advance()
    {
        if (pos < tokens.size()) return tokens[pos++];
        return token("eof", "UnexpectedEOF", 0);
    }
    bool isAtEnd() const
    {
        return pos >= tokens.size() || tokens[pos].type == "eof";
    }
    bool check(const string& type) const
    {
        if (isAtEnd()) return false;
        return peekToken().type == type;
    }
    bool match(const string& type)
    {
        if (check(type))
        {
            advance();
            return true;
        }

        return false;
    }
    void expect(const string& type, ParseError err, string token_ = "")
    {
        if (!check(type))
        {
            token t = peekToken();
            string Unexp_tok = t.val;
            string msg = "Error: ";

            switch (err)
            {
            case UnexpectedEOF: msg += "UnexpectedEOF" + Unexp_tok; break;
            case FailedToFindToken: msg += "FailedToFindToken" + Unexp_tok; break;
            case ExpectedTypeToken: msg += "ExpectedTypeToken" + Unexp_tok; break;
            case ExpectedIdentifier: msg += "ExpectedIdentifier" + Unexp_tok; break;
            case UnexpectedToken: msg += "UnexpectedToken" + Unexp_tok; break;
            case ExpectedFloatLit: msg += "ExpectedFloatLit" + Unexp_tok; break;
            case ExpectedIntLit: msg += "ExpectedIntLit" + Unexp_tok; break;
            case ExpectedStringLit: msg += "ExpectedStringLit" + Unexp_tok; break;
            case ExpectedBoolLit: msg += "ExpectedBoolLit" + Unexp_tok; break;
            case ExpectedExpr: msg += "ExpectedExpr" + Unexp_tok; break;
            }


            msg += " at line " + to_string(t.line_no) +
                " token='" + t.val + "' type=" + t.type;

            throw runtime_error(msg);
        }
        else {
            advance();
        }
    }

public:
    Parser(const string& filename) : pos(0)
    {
        tokens = lexer.GenerateTokens(filename);
        for (auto i : tokens)
            cout << i.type << " " << i.val << endl;        
        if (tokens.empty() || tokens.back().type != "eof")
            tokens.push_back(token("eof", "UnexpectedEOF", tokens.back().line_no));
    }

 /*   shared_ptr<Program> parseProgram()
    {
        auto program = make_shared<Program>();
        while (!isAtEnd() && peekToken().type != "eof")
        {
            program->functions.push_back(parseFunction());
        }
        return program;
    }*/
    void parseComment() {
        token t = peekToken();

        // If current token is comment start
        if (check("T_COMSTART")) {
            advance(); // consume /*
            bool foundEnd = false;

            while (!isAtEnd()) {
                if (check("T_COMEND")) {
                    foundEnd = true;
                    advance(); // consume */
                    break;
                }
                else {
                    advance(); // skip other tokens inside comment
                }
            }

            if (!foundEnd) {
                token tk = peekToken();
                ThrowError("Expected comment end '*/' before EOF", tk.line_no, tk.val, tk.type);
            }
        }
        // If we find comment end without start
        else if (check("T_COMEND")) {
            token tk = peekToken();
            ThrowError("Unexpected comment end '*/' without matching start", tk.line_no, tk.val, tk.type);
        }
        // Otherwise just move forward
        else {
            advance();
        }
    }

    shared_ptr<Program> parseProgram()
    {
        auto program = make_shared<Program>();

        while (!isAtEnd() && peekToken().type != "eof")
        {

            if (check("T_COMSTART") || check("T_COMEND")) {
                parseComment();
                continue;
            }
            token t = peekToken();
            // Check if this could be a global variable
            if (isTypeToken(t.type))
            {
                token next = peekNext();
                if (next.type == "T_IDENTIFIER")
                {
                    size_t save = pos;   // save current position
                    advance(); advance(); // skip type and identifier

                    if (check("T_ASSIGN") || check("T_SEMICOLON"))
                    {
                        // rollback to saved position
                        pos = save;

                        // parse as VarDeclStmt
                        StmtPtr stmt = parseVarDeclStmt();
                        auto varDecl = dynamic_pointer_cast<VarDeclStmt>(stmt);
                        if (!varDecl) {
                            throw runtime_error("Expected VarDeclStmt while parsing global variable");
                        }

                        program->globals.push_back(varDecl);
                        continue;
                    }

                    // rollback if not a global variable
                    pos = save;
                }
            }

            // Otherwise, must be a function
            program->functions.push_back(parseFunction());
        }

        return program;
    }


private:
    void ThrowError(string message, int line_no, string val, string type)
    {
        string final_messgae = message + to_string(line_no) + "\nError Type: " + type + "\nToken Found: " + val;
        throw runtime_error(final_messgae);
    }
    //We have defined out grammar here
    //====================================================================================
    // FunctionDecl → Type T_IDENTIFIER T_LPAREN Params T_RPAREN Block
    shared_ptr<FuncDecl> parseFunction()
    {
        auto fd = make_shared<FuncDecl>();

        token t = peekToken();
        if (!isTypeToken(t.type))
        {
            string message = "Expected function return type at line";
            ThrowError(message, t.line_no, t.val, t.type);

        }
        fd->retType = t.type;
        advance();


        if (!check("T_IDENTIFIER")) {
            token tk = peekToken();
            string message = "Expected identifier for function name at line ";
            ThrowError(message, tk.line_no, tk.val, tk.type);


        }
        fd->name = peekToken().val;
        token op = advance();

        expect("T_LPAREN", UnexpectedToken, op.val);


        if (!check("T_RPAREN")) {

            while (true) {
                Param p = parseParam();
                fd->params.push_back(p);
                if (check("T_COMMA")) {
                    advance();
                    continue;
                }
                else break;
            }
        }
        op = peekToken();
        expect("T_RPAREN", UnexpectedToken, op.val);


        fd->body = parseBlockStmt();
        return fd;
    }

    bool isTypeToken(const string& tt)
    {
        static vector<string> types = { "T_INT","T_FLOAT","T_DOUBLE","T_STRING","T_BOOL","T_VOID" };
        return find(types.begin(), types.end(), tt) != types.end();
    }

    // Param → Type T_IDENTIFIER
    Param parseParam()
    {
        token t = peekToken();
        if (!isTypeToken(t.type))
        {
            string message = "Expected type token in param at line ";
            ThrowError(message, t.line_no, t.val, t.type);
        }
        Param p;
        p.typeTok = t.type;
        advance();
        if (!check("T_IDENTIFIER"))
        {
            token tk = peekToken();
            string message = "Expected identifier in param at line ";
            ThrowError(message, tk.line_no, t.val, t.type);
        }
        p.name = peekToken().val;
        advance();
        return p;
    }

    // Block → T_LBRACE Stmt* T_RBRACE
    shared_ptr<BlockStmt> parseBlockStmt()
    {
        if (!check("T_LBRACE"))
        {
            token t = peekToken();
            string message = "Expected '{' at line ";
            ThrowError(message, t.line_no, t.val, t.type);

        }
        advance();
        auto block = make_shared<BlockStmt>();
        while (!check("T_RBRACE") && !isAtEnd())
        {
            block->stmts.push_back(parseStatement());
        }
        expect("T_RBRACE", UnexpectedToken);
        return block;
    }

    // Stmt → various
    StmtPtr parseStatement()
    {
        token t = peekToken();
        if (check("T_SEMICOLON")) {
            advance();
            return make_shared<ExprStmt>(nullptr); // represent an empty statement
        }
        /*if (check("T_SEMICOLON"))
        {
            advance();
            return make_shared<BlockStmt>()->stmts.empty() ? (StmtPtr)make_shared<ExprStmt>(nullptr) : nullptr;
        }*/

        if (check("T_RETURN")) return parseReturnStmt();
        if (check("T_IF")) return parseIfStmt();
        if (check("T_WHILE")) return parseWhileStmt();
        if (check("T_FOR")) return parseForStmt();
        if (check("T_LBRACE")) return parseBlockStmt();
        if (isTypeToken(t.type)) return parseVarDeclStmt();
        if (check("T_SEMICOLON")) 
        { 
            advance();
            return make_shared<ExprStmt>(nullptr); 
        }

        return parseExprStmt();
    }

    // ExprStmt → Expr T_SEMICOLON
    StmtPtr parseExprStmt()
    {
        auto e = parseExpr();
        expect("T_SEMICOLON", ExpectedExpr);
        return make_shared<ExprStmt>(e);
    }

    // ReturnStmt → T_RETURN Expr T_SEMICOLON
    StmtPtr parseReturnStmt()
    {
        advance();
        ExprPtr e = parseExpr();
        expect("T_SEMICOLON", ExpectedExpr);
        return make_shared<ReturnStmt>(e);
    }

    // IfStmt → T_IF T_LPAREN Expr T_RPAREN Stmt (T_ELSE Stmt)?
    StmtPtr parseIfStmt()
    {
        advance();
        expect("T_LPAREN", UnexpectedToken);
        ExprPtr cond = parseExpr();
        expect("T_RPAREN", UnexpectedToken);
        StmtPtr thenStmt = parseStatement();
        StmtPtr elseStmt = nullptr;
        if (check("T_ELSE")) {
            advance();
            elseStmt = parseStatement();
        }
        return make_shared<IfStmt>(cond, thenStmt, elseStmt);
    }

    // WhileStmt → T_WHILE T_LPAREN Expr T_RPAREN Stmt
    StmtPtr parseWhileStmt()
    {
        advance();
        expect("T_LPAREN", UnexpectedToken);
        ExprPtr cond = parseExpr();
        expect("T_RPAREN", UnexpectedToken);
        StmtPtr body = parseStatement();
        return make_shared<WhileStmt>(cond, body);
    }

    // ForStmt → T_FOR T_LPAREN ExprStmt ExprStmt Expr? T_RPAREN Stmt
    StmtPtr parseForStmt()
    {
        advance();
        expect("T_LPAREN", UnexpectedToken);

        StmtPtr init;
        if (check("T_SEMICOLON")) {
            advance();
            init = nullptr;
        }
        else if (isTypeToken(peekToken().type)) {
            init = parseVarDeclStmt(); // consumes semicolon
        }
        else {
            init = parseExprStmt();
        }
        // cond (ExprStmt)
        StmtPtr condStmt;
        if (check("T_SEMICOLON")) {
            advance();
            condStmt = nullptr;
        }
        else {
            condStmt = parseExprStmt();
        }
        // iter expression (optional)
        ExprPtr iter = nullptr;
        if (!check("T_RPAREN")) {
            // there is an expression (not semicolon)
            iter = parseExpr();
        }
        expect("T_RPAREN", UnexpectedToken);
        StmtPtr body = parseStatement();
        return make_shared<ForStmt>(init, condStmt, iter, body);
    }

    // VarDeclStmt → Type T_IDENTIFIER (T_ASSIGN Expr)? T_SEMICOLON
    StmtPtr parseVarDeclStmt()
    {
        token t = peekToken();
        string typeTok = t.type;
        advance();
        if (!check("T_IDENTIFIER"))
        {
            token tk = peekToken();
            string message = "Expected identifier after type at line ";
            ThrowError(message, tk.line_no, tk.val, tk.type);
        }
        string name = peekToken().val;
        advance();
        ExprPtr init = nullptr;
        if (check("T_ASSIGN")) 
        {
            advance();
            init = parseExpr();
        }
        expect("T_SEMICOLON", UnexpectedToken);
        return make_shared<VarDeclStmt>(typeTok, name, init);
    }

    // Expressions
    ExprPtr parseExpr()
    {
        return parseAssignment();
    }

    // Assignment → T_IDENTIFIER T_ASSIGN Assignment | OrExpr
    ExprPtr parseAssignment()
    {
        if ( check("T_IDENTIFIER")) 
        {

            token id = peekToken();
            if (peekNext().type == "T_ASSIGN") {

                advance();
                advance();
                ExprPtr rhs = parseAssignment();

                auto lhs = make_shared<IdentifierExpr>(id.val);
                return make_shared<BinaryExpr>(lhs, "T_ASSIGN", rhs);
            }
        }
        return parseOrExpr();
    }

    ExprPtr parseOrExpr()
    {
        ExprPtr left = parseAndExpr();
        while (check("T_OR")) 
        {
            string op = peekToken().type; advance();
            ExprPtr right = parseAndExpr();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    ExprPtr parseAndExpr()
    {
        ExprPtr left = parseEquality();
        while (check("T_AND")) 
        {
            string op = peekToken().type; advance();
            ExprPtr right = parseEquality();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    ExprPtr parseEquality()
    {
        ExprPtr left = parseRelational();
        while (check("T_EQ") || check("T_NEQ")) {
            string op = peekToken().type; advance();
            ExprPtr right = parseRelational();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    ExprPtr parseRelational()
    {
        ExprPtr left = parseAdd();
        while (check("T_LT") || check("T_GT") || check("T_LEQ") || check("T_GEQ"))
        {
            string op = peekToken().type; advance();
            ExprPtr right = parseAdd();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    ExprPtr parseAdd()
    {
        ExprPtr left = parseMul();
        while (check("T_PLUS") || check("T_MINUS"))
        {
            string op = peekToken().type; advance();
            ExprPtr right = parseMul();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    ExprPtr parseMul()
    {
        ExprPtr left = parseUnary();
        while (check("T_MULT") || check("T_DIV") || check("T_MOD"))
        {
            string op = peekToken().type; advance();
            ExprPtr right = parseUnary();
            left = make_shared<BinaryExpr>(left, op, right);
        }
        return left;
    }

    // Unary → (T_NOT | T_MINUS | T_PLUS | T_INC | T_DEC) Unary | Postfix
    ExprPtr parseUnary()
    {
        if (check("T_NOT") || check("T_MINUS") || check("T_PLUS") || check("T_INC") || check("T_DEC"))
        {
            string op = peekToken().type; advance();
            ExprPtr rhs = parseUnary();
            return make_shared<UnaryExpr>(op, rhs);
        }
        return parsePostfix();
    }

    // Postfix → Primary ( (T_LPAREN ArgList? T_RPAREN) | (T_INC | T_DEC) )*
    ExprPtr parsePostfix()
    {
        ExprPtr left = parsePrimary();
        while (true) 
        {
            if (check("T_LPAREN")) 
            {
                advance();
                auto call = make_shared<CallExpr>(left);
                if (!check("T_RPAREN")) 
                {
                    while (true) 
                    {
                        ExprPtr arg = parseExpr();
                        call->args.push_back(arg);
                        if (check("T_COMMA"))
                        { advance();
                            continue; 
                        }
                        break;
                    }
                }
                expect("T_RPAREN", UnexpectedToken);
                left = call;
                continue;
            }
            else if (check("T_INC") || check("T_DEC"))
            {
                string op = peekToken().type; advance();
                left = make_shared<PostfixExpr>(left, op);
                continue;
            }
            break;
        }
        return left;
    }

    // Primary → T_IDENTIFIER | T_NUMBER | T_FLOATLIT | T_STRING_LITERAL | T_CHAR_LITERAL | T_TRUE | T_FALSE | T_LPAREN Expr T_RPAREN
    ExprPtr parsePrimary()
    {
        token t = peekToken();
        if (check("T_IDENTIFIER")) {
            advance();
            return make_shared<IdentifierExpr>(t.val);
        }
        if (check("T_NUMBER")) {
            advance();
            return make_shared<IntLiteral>(t.val);
        }
        if (check("T_FLOATLIT")) {
            advance();
            return make_shared<FloatLiteral>(t.val);
        }
        if (check("T_STRING_LIT")) {
            advance();
            return make_shared<StringLiteral>(t.val);
        }
        if (check("T_CHAR_LIT")) {
            advance();
            return make_shared<CharLiteral>(t.val);
        }
        if (check("T_TRUE") || check("T_FALSE")) {
            advance();
            return make_shared<BoolLiteral>(t.val);
        }
        if (check("T_LPAREN")) {
            advance();
            ExprPtr e = parseExpr();
            expect("T_RPAREN", UnexpectedToken);
            return e;
        }
        string message = "Expected primary expression at line ";
        ThrowError(message, t.line_no, t.val, t.type);

        ostringstream o;
        o << "Expected primary expression at line " << t.line_no << "\n token='" << t.val << "\n type=" << t.type;
        throw runtime_error(o.str());
    }
};
