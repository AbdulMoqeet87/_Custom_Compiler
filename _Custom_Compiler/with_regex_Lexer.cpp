#include"with_regex_Lexer.h"
#include <iostream>
#include <regex>
#include <unordered_map>
#include<fstream>
using namespace std;



Lexer_regex::Lexer_regex() : master(
    // Master regex pattern to match all tokens
    "[a-zA-Z_][a-zA-Z0-9_]*|"
    "[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|"
    "[0-9]+|"
    "[0-9]+[a-zA-Z_]+[a-zA-Z0-9_]*|"
    "//|"
    "'(\\\\.|[^'\\\\])'|"
    "\"([^\"\\\\]|\\\\.)*\"|"
    "==|=|;|,|"
    "\\(|\\)|\\{|\\}|"
    ">>|<<|!=|<=|>=|<|>|"
    "/\\*|\\*/|"
    "\\+|\\-|\\*|/|"
    "&&|"
    "\\|\\||"
    "!|"
    "\\+\\+|"
    "--|"
    "%|:"

), curr_line(1), invalid_regexs{ regex("[0-9]+[a-zA-Z_]+[a-zA-Z0-9_]*") }
{
    // keywords
    is_comment = false;
    keywords["cout"] = "T_COUT";
    keywords["cin"] = "T_CIN";
    keywords["int"] = "T_INT";
    keywords["main"] = "T_MAIN";
    keywords["float"] = "T_FLOAT";
    keywords["double"] = "T_DOUBLE";
    keywords["string"] = "T_STRING";
    keywords["bool"] = "T_BOOL";
    keywords["return"] = "T_RETURN";
    keywords["if"] = "T_IF";
    keywords["else"] = "T_ELSE";
    keywords["while"] = "T_WHILE";
    keywords["for"] = "T_FOR";
    keywords["fn"] = "T_FUNCTION";
    keywords["true"] = "T_TRUE";
    keywords["false"] = "T_FALSE";
    keywords["void"] = "T_VOID";
    keywords["let"] = "T_LET";
    keywords["const"] = "T_CONST";
    keywords["struct"] = "T_STRUCT";
    keywords["break"] = "T_BREAK";
    keywords["continue"] = "T_CONTINUE";
    keywords["null"] = "T_NULL";
    keywords["new"] = "T_NEW";
    keywords["class"] = "T_CLASS";
    keywords["public"] = "T_PUBLIC";
    keywords["private"] = "T_PRIVATE";
    keywords["protected"] = "T_PROTECTED";
    keywords["static"] = "T_STATIC";
    keywords["import"] = "T_IMPORT";
    keywords["then"] = "T_THEN";

    //regex defined for each token

    token_patterns["T_COMSTART"] = regex("/\\*");
    token_patterns["T_COMEND"] = regex("\\*/");
    token_patterns["T_IDENTIFIER"] = regex("[a-zA-Z_][a-zA-Z0-9_]*");

    // Literal patterns
    token_patterns["T_FLOAT_LIT"] = regex("[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?");
    token_patterns["T_STRING_LIT"] = regex("\".*?\"");
    token_patterns["T_CHAR_LIT"] = regex("'.'");
    token_patterns["T_NUMBER"] = regex("[0-9]+");

    // Operator patterns
    token_patterns["T_ASSIGN"] = regex("=");
    token_patterns["T_PLUS"] = regex("\\+");
    token_patterns["T_MINUS"] = regex("-");
    token_patterns["T_MULT"] = regex("\\*");
    token_patterns["T_DIV"] = regex("/");
    token_patterns["T_MOD"] = regex("%");
    token_patterns["T_LT"] = regex("<");
    token_patterns["T_GT"] = regex(">");
    token_patterns["T_NOT"] = regex("!");
    token_patterns["T_EQ"] = regex("==");
    token_patterns["T_NEQ"] = regex("!=");
    token_patterns["T_LEQ"] = regex("<=");
    token_patterns["T_GEQ"] = regex(">=");
    token_patterns["T_INC"] = regex("\\+\\+");
    token_patterns["T_DEC"] = regex("--");
    token_patterns["T_AND"] = regex("&&");
    token_patterns["T_OR"] = regex("\\|\\|");
    token_patterns["T_RSHIFT"] = regex(">>");
    token_patterns["T_LSHIFT"] = regex("<<");

    // Delimiter patterns
    token_patterns["T_LPAREN"] = regex("\\(");
    token_patterns["T_RPAREN"] = regex("\\)");
    token_patterns["T_LBRACE"] = regex("\\{");
    token_patterns["T_RBRACE"] = regex("\\}");
    token_patterns["T_LBRACKET"] = regex("\\[");
    token_patterns["T_RBRACKET"] = regex("\\]");
    token_patterns["T_COLON"] = regex(":");
    token_patterns["T_QUESTION"] = regex("\\?");

    token_patterns["T_COMMA"] = regex(",");
    token_patterns["T_SEMICOLON"] = regex(";");
    token_patterns["T_DOT"] = regex("\\.");
    token_patterns["T_ARROW"] = regex("->");
    token_patterns["T_Colon"] = regex(":");


}
vector<token> Lexer_regex::GenerateTokens(const string& file_name)
{

    ifstream rdr(file_name);
    if (!rdr)
        throw runtime_error("Could not open file!");


    string code;
    while (getline(rdr, code))
    {
        sregex_iterator iter(code.begin(), code.end(), master);
        sregex_iterator end;

        while (iter != end)
        {
            string line = iter->str();
            if (line == "//")
                break;

            if (is_comment && line == "*/")
                is_comment = false;
            if (!is_comment)
            {
                try
                {
                    IsInvalidLexeme(line);
                }
                catch (const runtime_error& e)
                {
                    cout << "Error caught: " << e.what() << line << " at line NO: " << curr_line << endl;
                    exit(0);
                }

                if (keywords.find(line) != keywords.end())
                {
                    token temp(keywords[line], line, curr_line);
                    tokens.push_back(temp);
                }
                else
                {
                    for (auto& entry : token_patterns)
                    {
                        if (regex_match(line, entry.second))
                        {
                            if (IsCommentStarting(entry.first))
                            {
                                is_comment = true;

                            }
                            token temp(entry.first, line, curr_line);
                            tokens.push_back(temp);
                            break;
                        }
                    }
                }
            }
            iter++;
        }
        curr_line++;
    }
    return tokens;
}
void Lexer_regex::IsInvalidLexeme(const string& Lexeme)
{
    /*regex r = regex("[0-9]+[a-zA-Z_][a-zA-Z0-9_]*|");
    if (regex_match(Lexeme, r))
        return;*/
    for (auto i : invalid_regexs)
    {
        if (regex_match(Lexeme, i))
        {
            throw runtime_error(" Invalid string ");
        }
    }
}

vector<token> Lexer_regex::getTokens()
{
    return tokens;
}

void Lexer_regex::PrintTokens()
{
    for (auto i : tokens)
    {
        cout << i.type << " -> " << i.val << endl;

    }
}


bool Lexer_regex::IsCommentStarting(const string& s)
{
    if (s == "T_COMSTART")
        return true;
    return false;
}
bool Lexer_regex::IsCommentEnding(const string& s)
{
    if (s == "T_COMEND")
        return true;
    return false;
}