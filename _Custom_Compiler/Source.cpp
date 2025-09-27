#include<iostream>
#include "Without_regex_Lexer.h"
#include"with_regex_Lexer.h"

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

//int main()
//{
//    const string code = "fn int my_fn(int x, float y) {string my_str = \"hmm\";bool my_bool = x == 40;return x;}";
//
//    Lexer_regex Lex;
//    Lex.GenerateTokens("text.txt");
//    Lex.PrintTokens();
//   
//    return 0;
//}
