#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct Expr;
struct Stmt;

struct Expr
{
    virtual ~Expr() {}
    virtual void print(int indent = 0) const = 0; // indent is for better printing
};

struct LiteralExpr : Expr
{
    string value;
    string type; // int, float, string, bool, short, long, char, longlong
    LiteralExpr(const string& v, const string& t) : value(v), type(t) {}
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "Literal(" << type << ": " << value << ")\n";
    }
};

struct IdentifierExpr : Expr
{
    string name; // variable name, function name
    IdentifierExpr(const string& n) : name(n) {}
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "Identifier(" << name << ")\n";
    }
};

struct BinaryExpr : Expr  // e.g., a + b, x * y that requires two operands
{
    string op;
    Expr* left;
    Expr* right;
    BinaryExpr(Expr* l, const string& o, Expr* r) : left(l), op(o), right(r) {}
    ~BinaryExpr()
    {
        delete left;
        delete right;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "BinaryExpr(" << op << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct Stmt
{
    virtual ~Stmt() {}
    virtual void print(int indent = 0) const = 0;
};

struct ExprStmt : Stmt // expression used as a statement, e.g., function call
{
    Expr* expr;
    ExprStmt(Expr* e) : expr(e) {}
    ~ExprStmt()
    {
        delete expr;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "ExprStmt\n";
        expr->print(indent + 2);
    }
};

struct BreakStmt : Stmt // break statement
{


    BreakStmt() {}

    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "BreakStmt\n";
    }
};

struct IfStmt : Stmt // if statement with optional else
{
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch; // can be nullptr if no else
    IfStmt(Expr* cond, Stmt* thenBr, Stmt* elseBr = nullptr)
        : condition(cond), thenBranch(thenBr), elseBranch(elseBr) {
    }
    ~IfStmt()
    {
        delete condition;
        delete thenBranch;
        delete elseBranch;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "IfStmt\n";
        cout << string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "Then:\n";
        thenBranch->print(indent + 4);
        if (elseBranch)
        {
            cout << string(indent + 2, ' ') << "Else:\n";
            elseBranch->print(indent + 4);
        }
    }
};

struct WhileStmt : Stmt // while loop
{
    Expr* condition;
    Stmt* body;
    WhileStmt(Expr* cond, Stmt* b) : condition(cond), body(b) {}
    ~WhileStmt()
    {
        delete condition;
        delete body;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "WhileStmt\n";
        cout << string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "Body:\n";
        body->print(indent + 4);
    }
};

struct ReturnStmt : Stmt // return statement
{
    Expr* value; // can be nullptr for void return
    ReturnStmt(Expr* val = nullptr) : value(val) {}
    ~ReturnStmt()
    {
        delete value;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "ReturnStmt\n";
        if (value)
        {
            value->print(indent + 2);
        }
    }
};

struct ForStmt : Stmt // for loop
{
    Stmt* initializer; // e.g int i = 0;
    Expr* condition;   // e.g i < 10;
    Expr* update;   // e.g i++ or i+=1;
    Stmt* body;       // loop body
    ForStmt(Stmt* init, Expr* cond, Expr* incr, Stmt* b)
        : initializer(init), condition(cond), update(incr), body(b) {}
    ~ForStmt()
    {
        delete initializer;
        delete condition;
        delete update;
        delete body;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "ForStmt\n";
        if (initializer)
        {
            cout << string(indent + 2, ' ') << "Initializer:\n";
            initializer->print(indent + 4);
        }
        if (condition)
        {
            cout << string(indent + 2, ' ') << "Condition:\n";
            condition->print(indent + 4);
        }
        if (update)
        {
            cout << string(indent + 2, ' ') << "Increment:\n";
            update->print(indent + 4);
        }
        cout << string(indent + 2, ' ') << "Body:\n";
        body->print(indent + 4);
    }
};

struct SwitchStmt : Stmt // switch statement
{
    Expr* expression;
    vector<pair<Expr*, Stmt*>> cases; // pairs of case value and corresponding statement
    Stmt* defaultCase; // can be nullptr if no default case
    SwitchStmt(Expr* expr, const vector<pair<Expr*, Stmt*>>& cs, Stmt* def = nullptr)
        : expression(expr), cases(cs), defaultCase(def) {}
    ~SwitchStmt()
    {
        delete expression;
        for (auto& c : cases)
        {
            delete c.first;
            delete c.second;
        }
        delete defaultCase;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "SwitchStmt\n";
        cout << string(indent + 2, ' ') << "Expression:\n";
        expression->print(indent + 4);
        for (const auto& c : cases)
        {
            cout << string(indent + 2, ' ') << "Case:\n";
            c.first->print(indent + 4);
            cout << string(indent + 2, ' ') << "Body:\n";
            c.second->print(indent + 4);
        }
        if (defaultCase)
        {
            cout << string(indent + 2, ' ') << "Default:\n";
            defaultCase->print(indent + 4);
        }
    }
};

struct Block : Stmt // a sequence of statements in e.g if, while, function body
{
    vector<Stmt*> statements;
    ~Block()
    {
        for (auto s : statements)
            delete s;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "Block\n";
        for (auto stmt : statements)
            stmt->print(indent + 2);
    }
};

struct Program // complete source code
{
    vector<Stmt*> statements;
    ~Program()
    {
        for (auto s : statements)
            delete s;
    }
    void print() const
    {
        cout << "Program\n";
        for (auto stmt : statements) stmt->print(2);
    }
};

struct VarDecl : Stmt
{
    string type;
    string name;
    Expr* initializer;
    VarDecl(const string& t, const string& n, Expr* init = nullptr)
        : type(t), name(n), initializer(init) {
    }

    ~VarDecl()
    {
        delete initializer;
    }
    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "VarDecl(" << type << " " << name << ")\n";
        if (initializer)
        {
            initializer->print(indent + 2);
        }
    }
};


struct funcDecl : Stmt
{
    string name;
    vector<string> params;
    Block* body;

    funcDecl(const string& n, const vector<string>& p, Block* b = nullptr)
        : name(n), params(p), body(b) {}

    ~funcDecl() 
    { 
        delete body; 
    }

    void print(int indent = 0) const override
    {
        cout << string(indent, ' ') << "funcDecl(" << name << ")\n";
        cout << string(indent + 2, ' ') << "Params: ";
        for (auto& p : params) cout << p << " ";
        cout << "\n";
        body->print(indent + 2);
    }
};