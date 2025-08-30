#pragma once
#include<vector>
#include<iostream>
#include<string>
#include <unordered_map>
using namespace std;


struct Token
{
    string type;
    string value;
    int line;
    int column;
    Token(string t, string v, int l, int c):type(t), value(v), line(l), column(c){}
    Token(){}
};


class Without_regex_Lexer
{
    
    vector<Token> tokens;
    unordered_map<string, string> keywords;
    int line_number = 1;
    int column_number = 1;
    bool isComment = false;
    public:
        Without_regex_Lexer();
        bool isKeyword(const string& word);
        string getKeywordToken(const string& word);
        vector<Token> CreateTokens(const string& filename);
        void Tokenize(const string& input);
        bool isIgnoreChar(char c);
        bool isAlpha(char c);
        bool isNum(char c);
        //void Tokenize(const string& input);
        void printTokens();
};