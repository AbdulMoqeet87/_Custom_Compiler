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