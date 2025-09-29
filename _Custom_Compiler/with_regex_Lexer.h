#pragma once
#include<vector>
#include<iostream>
#include<unordered_map>
#include<string.h>
#include<regex>
using namespace std;

struct token 
{
	string type;
	string val;
	int line_no;
	token(string t, string v, int l)
	{
		type = t, val = v, line_no = l; 
	}
	token() 
	{
		type = "", val = "", line_no = -1; 
	}
};

class Lexer_regex
{
	vector<token>tokens;
	int curr_line;
	unordered_map<string, string> keywords;
	unordered_map<string, regex> token_patterns;
	vector<regex>invalid_regexs;
	regex master;
	bool is_comment;

public:
	Lexer_regex();
	vector<token> getTokens();
	vector<token> GenerateTokens(const string& code);
	void IsInvalidLexeme(const string& Lexeme);
	void PrintTokens();
	bool IsCommentStarting(const string& s);
	bool IsCommentEnding(const string& s);
};