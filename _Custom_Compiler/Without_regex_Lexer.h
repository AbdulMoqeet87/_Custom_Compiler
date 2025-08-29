#pragma once
#include<vector>
#include<iostream>
#include<string.h>
using namespace std;


struct Token
{
    int type;
    string value;
    int line;
    int column;
    Token(int t, string v, int l, int c):type(t), value(v), line(l), column(c){}
    Token(){}
};


class Without_regex_Lexer
{
    vector<Token> tokens;
    public:
        Without_regex_Lexer();
        vector<Token> CreateTokens(const string& input);
};