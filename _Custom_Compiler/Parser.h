#pragma once
#include <vector>
#include "with_regex_Lexer.h"
#include "AST.h"
#include "ParseError.h"

class Parser
{
    vector<token> tokens;
    size_t current = 0;
public:
    Parser(const vector<token>& t) : tokens(t) {}
    Program* parseProgram();
private:
    // Helpers
    token peek();
    token advance();
    bool check(const string& type);
    bool match(const vector<string>& types);
    bool isAtEnd();

    // Grammar rules
    Expr* expression(); // like e.g a + b * c
    Expr* equality(); // like e.g a == b, a != b
    Expr* term(); // like e.g a + b, a - b
    Expr* factor(); // like e.g a * b, a / b
    Expr* primary(); // like e.g 123, 45.67, "hello", true, false, identifier

    Stmt* declaration(); // 
    Block* block();
    Stmt* varDeclaration();
    Stmt* funcDeclaration();
    vector<string> paramaeters();
    vector<string> type();
};