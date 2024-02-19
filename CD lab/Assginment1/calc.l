%{
#include <stdlib.h>
void yyerror(char *);
#include "y.tab.h" 
%}

%%

[0-9]+  {
            yylval = atoi(yytext);
            return INTEGER;
        }

[-+*/()\n]    return *yytext;

[ \t]   ;

X       exit(0);

.       yyerror("Invalid Character!");

%%

int yywrap(void) {
    return 1;
}
