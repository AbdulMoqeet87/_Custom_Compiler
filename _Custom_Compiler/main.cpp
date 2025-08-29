#include<iostream>
#include "Without_regex_Lexer.h"
using namespace std;

int main()
{
    Without_regex_Lexer lexer;
    string input = "int";
    string result = lexer.getKeywordToken(input);
    cout << "The token for '" << input << "' is: " << result << endl;
    vector<Token> tokens = lexer.CreateTokens("text.txt");
    lexer.printTokens();
    return 0;
}