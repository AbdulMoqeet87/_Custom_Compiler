#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "Parser2.h" 

using namespace std;

enum ScopeError {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition,
};

struct Symbol
{
    string name;
    bool isFunction = false;
    string type;
    Symbol() = default;
    Symbol(const string& n, const string& t, bool isFunc)
        : name(n), type(t), isFunction(isFunc) {}
};

struct Scope
{
    unordered_map<string, Symbol> table;
    shared_ptr<Scope> parent;

    Scope(shared_ptr<Scope> parent = nullptr) : parent(parent) {}

    // declare symbol only in this scope , returns false on redefinition in same scope
    bool declareSym(const Symbol& s) {
        if (table.find(s.name) != table.end()) return false;
        table[s.name] = s;
        return true;
    }

    // lookup from this scope upward
    const Symbol* lookup(const string& name) const {
        const Scope* cur = this;
        while (cur) {
            auto it = cur->table.find(name);
            if (it != cur->table.end()) return &it->second;
            cur = cur->parent.get();
        }
        return nullptr;
    }

    // check only this scope for existence
    bool existsInThisScope(const string& name) const {
        return table.find(name) != table.end();
    }

};


class ScopeAnalizer
{
    Parser parser;
    shared_ptr<Scope> globalScope;
    shared_ptr<Scope> currentScope;

public:
    ScopeAnalizer(const string& filename) :parser(filename) {
        globalScope = make_shared<Scope>(nullptr);
        currentScope = globalScope;
    }

    void analyzeProgram() {
        auto program = parser.parseProgram();
        if (!program) {
            cerr << "Parsing failed. Cannot perform scope analysis.\n";
            return;
        }
        cout << "Parsed Program AST:\n";
        program->print();
        cout << "Scope Analysis Starting.\n";
        for (const auto& item : program->globalItems)
        {
            if (auto func = dynamic_pointer_cast<FuncDecl>(item)) 
            {
                // Handle function declaration
                if (!globalScope->declareSym(Symbol(func->name, func->retType, true))) 
                {
                    reportError(FunctionPrototypeRedefinition, func->name);                    
                }
                analyzeFunction(func);
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item)) 
            {
                // Handle global variable declaration
                if (!globalScope->declareSym(Symbol(var->name, var->typeTok, false))) 
                {
                    reportError(VariableRedefinition, var->name);                   
                }
                if (var->init)
                    analyzeExpr(var->init);
            }
        }
        

    }

private:
    // push a new scope
    void pushScope() {
        currentScope = make_shared<Scope>(currentScope);
    }

    // pop current scope
    void popScope() {
        if (!currentScope->parent) {
            currentScope = globalScope;
            return;
        }
        currentScope = currentScope->parent;
    }

    void analyzeFunction(const shared_ptr<FuncDecl>& func) {
        pushScope();

        for (const auto& param : func->params) {
            if (!currentScope->declareSym(Symbol(param.name, param.typeTok, false))) {
                reportError(VariableRedefinition, param.name);
            }
        }
        if (func->body) {
            analyzeBlockStmt(func->body, true);
        }
        popScope();
    }
    void analyzeBlockStmt(const shared_ptr<BlockStmt>& block, bool isFunctionBody = false) {
        if (!isFunctionBody)
            pushScope();

        for (const auto& stmt : block->stmts) {
            analyzeStmt(stmt);
        }
        popScope();
    }

    void analyzeStmt(const shared_ptr<Stmt>& stmt) {
        if (!stmt) return;

        // dynamic dispatch based on actual derived type:
        if (auto bs = dynamic_pointer_cast<BlockStmt>(stmt)) {
            analyzeBlockStmt(bs);
            return;
        }
        if (auto vd = dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            // declare variable in current scope
            if (!currentScope->declareSym(Symbol(vd->name, vd->typeTok, false))) {
                reportError(ScopeError::VariableRedefinition, vd->name);
            }
            // analyze initializer expression if present
            if (vd->init) analyzeExpr(vd->init);
            return;
        }
        if (auto rs = dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (rs->expr) analyzeExpr(rs->expr);
            return;
        }
        if (auto es = dynamic_pointer_cast<ExprStmt>(stmt)) {
            if (es->expr) analyzeExpr(es->expr);
            return;
        }
        if (auto ifs = dynamic_pointer_cast<IfStmt>(stmt)) {
            if (ifs->cond) analyzeExpr(ifs->cond);
            if (ifs->thenStmt) analyzeStmt(ifs->thenStmt);
            if (ifs->elseStmt) analyzeStmt(ifs->elseStmt);
            return;
        }
        if (auto ws = dynamic_pointer_cast<WhileStmt>(stmt)) {
            if (ws->cond) analyzeExpr(ws->cond);
            if (ws->body) analyzeStmt(ws->body);
            return;
        }
        if (auto fs = dynamic_pointer_cast<ForStmt>(stmt)) {
            if (fs->init) analyzeStmt(fs->init);
            if (fs->condStmt) analyzeStmt(fs->condStmt);
            if (fs->iterExpr) analyzeExpr(fs->iterExpr);
            if (fs->body) analyzeStmt(fs->body);
            return;
        }

        // unknown stmt type -> ignore (or throw)
    }

    // analyze an expression
    void analyzeExpr(const std::shared_ptr<Expr>& e) {
        if (!e) return;

        if (auto id = dynamic_pointer_cast<IdentifierExpr>(e)) {
            // lookup identifier as variable or parameter or function (we only check existence)
            const Symbol* found = currentScope->lookup(id->name);
            if (!found) {
                // name not found -> undeclared variable accessed
                reportError(ScopeError::UndeclaredVariableAccessed, id->name);
            }
            return;
        }

        if (auto il = dynamic_pointer_cast<IntLiteral>(e)) {
            return;
        }
        if (auto fl = dynamic_pointer_cast<FloatLiteral>(e)) {
            return;
        }
        if (auto sl = dynamic_pointer_cast<StringLiteral>(e)) {
            return;
        }
        if (auto bl = dynamic_pointer_cast<BoolLiteral>(e)) {
            return;
        }
        if (auto cl = dynamic_pointer_cast<CharLiteral>(e)) {
            return;
        }

        if (auto unary = dynamic_pointer_cast<UnaryExpr>(e)) {
            if (unary->rhs) analyzeExpr(unary->rhs);
            return;
        }

        if (auto binary = dynamic_pointer_cast<BinaryExpr>(e)) {
            if (binary->left) analyzeExpr(binary->left);
            if (binary->right) analyzeExpr(binary->right);
            return;
        }

        if (auto call = dynamic_pointer_cast<CallExpr>(e)) {
            // The callee can be an identifier expression (most common); try to resolve function name
            // If callee is an Identifier, check that identifier exists and is a function
            if (auto calleeId = dynamic_pointer_cast<IdentifierExpr>(call->callee)) {
                const Symbol* sym = currentScope->lookup(calleeId->name);
                if (!sym) {
                    reportError(ScopeError::UndefinedFunctionCalled, calleeId->name);
                }
                else if (!sym->isFunction) {
                    // name exists but not a function -> undefined function called
                    reportError(ScopeError::UndefinedFunctionCalled, calleeId->name);
                }
                else {
                    // ok (we don't check arg counts/types here)
                }
            }
            else {
                // callee is an expression (e.g., more complex); analyze it anyway
                analyzeExpr(call->callee);
            }

            // analyze args
            for (auto& arg : call->args) if (arg) analyzeExpr(arg);
            return;
        }

        if (auto post = dynamic_pointer_cast<PostfixExpr>(e)) {
            if (post->base) analyzeExpr(post->base);
            return;
        }

        // other expression types: ignore or recursively inspect members if added in future.
    }

    void reportError(ScopeError err, const string& name) {
        switch (err) {
        case UndeclaredVariableAccessed:
            cerr << "[ScopeError] UndeclaredVariableAccessed: " << name << "\n";
            break;
        case UndefinedFunctionCalled:
            cerr << "[ScopeError] UndefinedFunctionCalled: " << name << "\n";
            break;
        case VariableRedefinition:
            cerr << "[ScopeError] VariableRedefinition: " << name << "\n";
            break;
        case FunctionPrototypeRedefinition:
            cerr << "[ScopeError] FunctionPrototypeRedefinition: " << name << "\n";
            break;
        }
        return;
    }
};