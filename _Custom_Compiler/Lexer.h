#pragma once
#include<vector>
#include<iostream>
#include<string.h>
using namespace std;

class Lexer
{
	vector<string>tokens;
	vector<string>key_words;
public:
	Lexer();
	void LoadKeyWordsFromFile(const string file_name);
	vector<string>CreateTokens();

};

