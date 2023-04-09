%{
#include <stdio.h>
#include <math.h>
#include "y.tab.h"
%}

%option yylineno
alpha	[a-zA-Z]
alphanum [a-zA-Z0-9]
alphanum_us 	[a-zA-Z0-9_]
num     [0-9]

%%
";"     { printf("%c ", yytext[0]); return yytext[0]; }
"="     { printf("%c ", yytext[0]); return yytext[0]; }

(\+|\-|\/|\*)    { printf("%c ", yytext[0]); return yytext[0]; }
(\(|\)|\{|\})    { printf("%c ", yytext[0]); return yytext[0]; }
(<|>|<=|>=|==)      { printf("COMP "); return COMP; }

"extern"    { printf("EXTERN "); return EXTERN; }
"int"   { printf("INT "); return INT; }
"void"      { printf("VOID "); return VOID; }
"return"    {printf("RETURN "); return RETURN; }

"if"    { printf("IF "); return IF; }
"else"  { printf("ELSE "); return ELSE; }
"for"   { printf("FOR "); return FOR; }
"while" { printf("WHILE "); return WHILE; }

"print"     { printf("PRINT "); return PRINT; }
"read"      { printf("READ "); return READ; }

{alpha}{alphanum_us}*{alphanum}+|{alpha}    { printf("NAME "); return NAME; }
{num}+      {printf("NUM "); return NUM; }
%%

int yywrap(){
	return 1;
}

/*
int main() {
    yylex();
    return(0);
} */
