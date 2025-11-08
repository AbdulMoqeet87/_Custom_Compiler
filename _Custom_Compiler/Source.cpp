#include<iostream>
//#include "Without_regex_Lexer.h"
#include"with_regex_Lexer.h"
#include"Parser.h"
//#include"Parser2.h"

using namespace std;


int main() {
    // 1. run the lexer
    Lexer_regex lexer;
	lexer.GenerateTokens("text.txt");

    cout << "tokens:\n";
    lexer.PrintTokens();

    // 2. run the parser
    vector<token> tokens = lexer.getTokens();
	Parser parser(tokens);
	parser.parseProgram()->print();

    return 0;
}

//int main()
//{
//    Without_regex_Lexer lexer;
//    string input = "int";
//    string result = lexer.getKeywordToken(input);
//    cout << "The token for '" << input << "' is: " << result << endl;
//    vector<Token> tokens = lexer.CreateTokens("text.txt");
//    lexer.printTokens();
//    return 0;
//}

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


int main()
{
	try
	{
		Parser p("text.txt");
		auto pars = p.parseProgram();
		pars->print();

	}
	catch (const std::exception& e)
	{
		cout << e.what();
	}

	return 0;

}