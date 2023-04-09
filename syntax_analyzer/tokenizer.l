%{
#include <stdio.h>
#include <math.h>
//#include "y.tab.h"
%}

%option yylineno
alpha	[a-zA-Z]
alphanum [a-zA-Z0-9]
alphanum_us 	[a-zA-Z0-9_]
num     [0-9]

%%
";"     { printf("%c ", yytext[0]);}
"="     { printf("%c ", yytext[0]);}

(\+|\-|\/|\*)    { printf("%c ", yytext[0]);}
(\(|\)|\{|\})    { printf("%c ", yytext[0]);}
(<|>|<=|>=|==)      { printf("COMP ");}

"extern"    { printf("EXTERN ");}
"int"   { printf("INT ");}
"void"      { printf("VOID ");}
"return"    {printf("RETURN ");}

"if"    { printf("IF ");}
"else"  { printf("ELSE ");}
"for"   { printf("FOR ");}
"while" { printf("WHILE "); }

"print"     { printf("PRINT ");}
"read"      { printf("READ ");}

{alpha}{alphanum_us}*{alphanum}+|{alpha}    { printf("NAME ");}
{num}+      {printf("NUM ");}
%%

int yywrap(){
	return 1;
}

/*
int main() {
    yylex();
    return(0);
} */
