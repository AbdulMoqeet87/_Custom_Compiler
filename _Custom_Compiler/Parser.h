//#pragma once
//#include <vector>
//#include "with_regex_Lexer.h"
//#include "AST.h"
//#include "ParseError.h"
//
//class Parser
//{
//    vector<token> tokens;
//    size_t current = 0;
//public:
//    Parser(const vector<token>& t) : tokens(t) {}
//    Program* parseProgram();
//private:
//    // Helpers
//    token peek();
//    token advance();
//    bool check(const string& type);
//    bool match(const vector<string>& types);
//    bool isAtEnd();
//
//    // Grammar rules
//    Stmt* declaration();
//    Block* block();
//    Stmt* varDeclaration();
//    string parseReturnType();
//    vector<string> parameters();
//    Stmt* statement();
//	Stmt* exprStatement();
//    Stmt* ifStatement();
//	Stmt* whileStatement();
//	Stmt* returnStatement();
//	Stmt* forStatement();
//	Stmt* breakStatement();
//    Stmt* switchStatement();
//    Stmt* continueStatement();
//    Expr* expression(); // like e.g a + b * c
//	Expr* assignment();
//	Expr* logicalOr(); // like e.g a || b
//	Expr* logicalAnd(); // like e.g a && b
//	Expr* comparison(); // like e.g a < b, a > b, a <= b, a >= b
//    Expr* equality(); // like e.g a == b, a != b
//    Expr* term(); // like e.g a + b, a - b
//    Expr* factor(); // like e.g a * b, a / b
//	Expr* unary(); // like e.g -a, !b
//	Expr* call(); // function call
//	vector<Expr*> arguments();
//    Expr* primary(); // like e.g 123, 45.67, "hello", true, false, identifier
//};