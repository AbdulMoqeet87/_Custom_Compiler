%{
#include <stdio.h>
%}

%%
int             { printf("T_INT\n"); }
[0-9]+          { printf("T_NUMBER(%s)\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*   { printf("T_IDENTIFIER(%s)\n", yytext); }
"="             { printf("T_ASSIGN\n"); }
";"             { printf("T_SEMICOLON\n"); }
[ \t\n]         ;   // ignore whitespace
.               { printf("Unknown: %s\n", yytext); }
%%

int main() {
    yylex();
    return 0;
}
