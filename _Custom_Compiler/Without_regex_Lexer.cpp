#include "Without_regex_Lexer.h"
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

Without_regex_Lexer::Without_regex_Lexer()
{
    keywords["int"] = "T_INT";
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
    keywords["switch"] = "T_SWITCH";
    keywords["case"] = "T_CASE";
    keywords["default"] = "T_DEFAULT";
    keywords["enum"] = "T_ENUM";
}

vector<Token> Without_regex_Lexer::CreateTokens(const string &filename)
{
    ifstream file(filename);
    if (!file)
        throw runtime_error("Could not open file!");

    string line;
    while (getline(file, line))
    {
        column_number = 1;
        Tokenize(line);
        line_number++;
    }
    return tokens;
}

bool Without_regex_Lexer::isKeyword(const string &word)
{
    return keywords.find(word) != keywords.end();
}

string Without_regex_Lexer::getKeywordToken(const string &word)
{
    if (isKeyword(word))
    {
        return keywords[word];
    }
    return "T_IDENTIFIER";
}

bool Without_regex_Lexer::isIgnoreChar(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Without_regex_Lexer::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Without_regex_Lexer::isNum(char c)
{
    return c >= '0' && c <= '9';
}

void Without_regex_Lexer::Tokenize(const string &input)
{
    for (int i = 0; i < input.size(); i++)
    {
        char currentChar = input[i];
        //cout<<"char at "<<i<<" is "<<currentChar<<endl;

        if(isComment==true)
        {
            while (i < input.size() && input[i] != '*')
            {
                i++;
                column_number++;
            }
            if(i + 1 < input.size() && input[i + 1] == '/')
            {
                isComment = false;
                i++;
                column_number++;
            }
            continue;
        }

        if (isIgnoreChar(currentChar))
        {
            if (currentChar == '\n')
            {
                line_number++;
                column_number = 1;
            }
            else
            {
                column_number++;
            }
            continue;
        }

        if (isAlpha(currentChar) || currentChar == '_')
        {
            string word;
            int sci = column_number;
            while (i < input.size() && (isalnum(input[i]) || input[i] == '_'))
            {
                word += input[i];
                i++;
                column_number++;
            }
            i--;
            string tokenType = getKeywordToken(word);
            tokens.push_back({tokenType, word, line_number, sci});
            continue;
        }

        if (isNum(currentChar))
        {
            string number;
            int sci = column_number;
            bool isFloat = false;
            while (i < input.size() && isdigit(input[i]))
            {
                number += input[i];
                i++;
                column_number++;
            }
            if (i < input.size() && input[i] == '.')
            {
                number += input[i];
                i++;
                column_number++;
                while (i < input.size() && isdigit(input[i]))
                {
                    number += input[i];
                    i++;
                    column_number++;
                }
                isFloat =true;
                
            }
            if (i < input.size() && (input[i] == 'e' || input[i] == 'E'))
            {
                number += input[i];
                i++;
                column_number++;
                if (i < input.size() && (input[i] == '+' || input[i] == '-'))
                {
                    number += input[i];
                    i++;
                    column_number++;
                }
                while (i < input.size() && isdigit(input[i]))
                {
                    number += input[i];
                    i++;
                    column_number++;
                }
            }
            if (i < input.size() && (isAlpha(input[i]) || input[i] == '_'))
                throw runtime_error("Invalid number format at line " + to_string(line_number) + ", column " + to_string(column_number));
            i--;
            if(isFloat)
                tokens.push_back({"T_FLOATLIT", number, line_number, sci});
            else
            tokens.push_back({"T_INTLIT", number, line_number, sci});
            continue;
        }

        if (currentChar == '"')
        {
            string strLit;
            int sci = column_number;
            tokens.push_back({"T_QUOTE", "\"", line_number, column_number});
            i++;
            column_number++;
            while (i < input.size() && input[i] != '"')
            {
                if (input[i] == '\\' && i + 1 < input.size() && (input[i + 1] == '"' ||  input[i + 1] == 'n' || input[i + 1] == 't' || input[i + 1] == 'r'))
                {
                    strLit += input[i];
                    i++;
                    strLit += input[i];
                    i++;
                    column_number+=2;
                }
                else
                {
                strLit += input[i];
                i++;
                column_number++;
                }
            }
            if (i < input.size())
            {
               
                tokens.push_back({"T_STRINGLIT", strLit, line_number, sci});
                tokens.push_back({"T_QUOTE", "\"", line_number, column_number});

            }
            else
            {
                printTokens();
                throw runtime_error("Unterminated string literal at line " + to_string(line_number) + ", column " + to_string(sci));
            }
            continue;
        }

        switch (currentChar)
        {
        case '+':
            if (i + 1 < input.size() && input[i + 1] == '+')
            {
                tokens.push_back({"T_INCREMENT", "++", line_number, column_number});
                i++;
                column_number++;
            }
            else if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_PLUSEQ", "+=", line_number, column_number});
                i++;
                column_number++;
               
            }
            else
            {
                tokens.push_back({"T_PLUS", "+", line_number, column_number});
            }
            break;
        case '-':
            if (i + 1 < input.size() && input[i + 1] == '-')
            {
                tokens.push_back({"T_DECREMENT", "--", line_number, column_number});
                i++;
                column_number++;
            }
            else if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_MINUSEQ", "-=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_MINUS", "-", line_number, column_number});
            }
            break;
        case '*':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_MULTEQ", "*=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_MULTIPLY", "*", line_number, column_number});
            }

            break;
        case '/':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_DIVEQ", "/=", line_number, column_number});
                i++;
                column_number++;
            }
            else if( i + 1 < input.size() && input[i + 1] == '/')
            {
                i = input.size();
                break;
            }
            else if( i + 1 < input.size() && input[i + 1] == '*')
            {
                isComment = true;
                i = input.size();
                break;
            }
            else
            {
                tokens.push_back({"T_DIVIDE", "/", line_number, column_number});
            }
            break;
        case '=':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_EQUALSOP", "==", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_ASSIGNOP", "=", line_number, column_number});
            }
            break;
        case '!':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_NOTEQ", "!=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_NOT", "!", line_number, column_number});
            }
            break;
        case '<':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_LTE", "<=", line_number, column_number});
                i++;
                column_number++;
            }
            else if (i + 1 < input.size() && input[i + 1] == '<')
            {
                tokens.push_back({"T_SHL", "<<", line_number, column_number});
                i++;
                column_number++;
                if (i + 1 < input.size() && input[i + 1] == '=')
                {
                    tokens.push_back({"T_SHLEQ", "<<=", line_number, column_number});
                    i++;
                    column_number++;
                }
            }
            else
            {
                tokens.push_back({"T_LT", "<", line_number, column_number});
            }
            break;
        case '>':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_GTE", ">=", line_number, column_number});
                i++;
                column_number++;
            }
            else if( i + 1 < input.size() && input[i + 1] == '>')
            {
                tokens.push_back({"T_SHR", ">>", line_number, column_number});
                i++;
                column_number++;
                if( i + 1 < input.size() && input[i + 1] == '=')
                {
                    tokens.push_back({"T_SHREQ", ">>=", line_number, column_number});
                    i++;
                    column_number++;
                }
            }
            else
            {
                tokens.push_back({"T_GT", ">", line_number, column_number});
            }
            break;
        case '&':
            if (i + 1 < input.size() && input[i + 1] == '&') 
            {
                tokens.push_back({"T_AND", "&&", line_number, column_number});
                i++;
                column_number++;
            }
            else if( i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_ANDEQ", "&=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_AMPERSAND", "&", line_number, column_number});
            }
            break;
        case '|':
            if (i + 1 < input.size() && input[i + 1] == '|')
            {
                tokens.push_back({"T_OR", "||", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_BITOR", "|", line_number, column_number});
            }
            break;
        case ';':
            tokens.push_back({"T_SEMICOLON", ";", line_number, column_number});
            break;
        case ',':
            tokens.push_back({"T_COMMA", ",", line_number, column_number});
            break;
        case '(':
            tokens.push_back({"T_PARENL", "(", line_number, column_number});
            break;
        case ')':
            tokens.push_back({"T_PARENR", ")", line_number, column_number});
            break;
        case '{':
            tokens.push_back({"T_BRACEL", "{", line_number, column_number});
            break;
        case '}':
            tokens.push_back({"T_BRACER", "}", line_number, column_number});
            break;
        case '[':
            tokens.push_back({"T_BRACKETL", "[", line_number, column_number});
            break;
        case ']':
            tokens.push_back({"T_BRACKETR", "]", line_number, column_number});
            break;
        case '.':
            tokens.push_back({"T_DOT", ".", line_number, column_number});
            break;
        case ':':
            tokens.push_back({"T_COLON", ":", line_number, column_number});
            break;
        case '%':
            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_MODEQ", "%=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_MODULO", "%", line_number, column_number});
            }
            break;
        case '^':
            if( i + 1 < input.size() && input[i + 1] == '=')
            {
                tokens.push_back({"T_BITXOREQ", "^=", line_number, column_number});
                i++;
                column_number++;
            }
            else
            {
                tokens.push_back({"T_BITXOR", "^", line_number, column_number});
            }
            break;
        case '~':
            tokens.push_back({"T_BITNOT", "~", line_number, column_number});
            break;
        case '?':
            tokens.push_back({"T_QUESTION", "?", line_number, column_number});
            break;
        case '#':
            tokens.push_back({"T_HASH", "#", line_number, column_number});
            break;

        default:
            throw runtime_error("Unexpected character '" + string(1, currentChar) + "' at line " + to_string(line_number) + ", column " + to_string(column_number));
        }
    }
}

void Without_regex_Lexer::printTokens()
{
    cout << "[ ";
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == "T_IDENTIFIER" || tokens[i].type == "T_STRINGLIT")
        {
            cout << tokens[i].type << "(\"" << tokens[i].value << "\")";
        }
        else if (tokens[i].type == "T_INTLIT" || tokens[i].type == "T_FLOATLIT")
        {
            cout << tokens[i].type << "(" << tokens[i].value << ")";
        }
        else
        {
            cout << tokens[i].type;
        }
        if (i != tokens.size() - 1)
            cout << ", ";
    }
    cout << " ]" << endl;
}
