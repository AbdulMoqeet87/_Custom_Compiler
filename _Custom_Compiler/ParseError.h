#pragma once
#include <stdexcept>
#include <string>
#include "with_regex_Lexer.h"

using namespace std;

class ParseError : public runtime_error
{
public:
    ParseError(const string& msg)
        : runtime_error(msg) {
    }

    static ParseError UnexpectedEOF()
    {
        return ParseError("Unexpected end of file.");
    }
    static ParseError FailedToFindToken(const token& t)
    {
        return ParseError("Failed to find token near: " + t.val);
    }
    static ParseError ExpectedTypeToken()
    {
        return ParseError("Expected a type token.");
    }
    static ParseError ExpectedIdentifier()
    {
        return ParseError("Expected an identifier.");
    }
    static ParseError UnexpectedToken(const token& t)
    {
        return ParseError("Unexpected token: " + t.type + " (" + t.val + ")");
    }
    static ParseError ExpectedFloatLit()
    {
        return ParseError("Expected a float literal.");
    }
    static ParseError ExpectedIntLit()
    {
        return ParseError("Expected an int literal.");
    }
    static ParseError ExpectedStringLit()
    {
        return ParseError("Expected a string literal.");
    }
    static ParseError ExpectedBoolLit()
    {
        return ParseError("Expected a bool literal.");
    }
    static ParseError ExpectedExpr()
    {
        return ParseError("Expected an expression.");
    }
};
