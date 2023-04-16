%{
#include <stdio.h>
#include <string.h>
#include "./ast/ast.h"
#include "y.tab.h"
%}

%option yylineno
alpha	[a-zA-Z]
alphanum [a-zA-Z0-9]
alphanum_us 	[a-zA-Z0-9_]
num     [0-9]

%%
[ \t\n]
";"     { return yytext[0]; }
"="     { return yytext[0]; }

(\+|\-|\/|\*)    { return yytext[0]; }
(\(|\)|\{|\})    { return yytext[0]; }
"<"     { return LT; }
">"     { return GT; }
"<="    { return LE; }
">="    { return GE; }
"=="    { return EQ; }
"!="    { return NEQ; }

"extern"    { return EXTERN; }
"int"   { return INT; }
"void"      { return VOID; }
"return"    { return RETURN; }

"if"    { return IF; }
"else"  { return ELSE; }
"while" { return WHILE; }

"print"     { yylval.string = strdup("Print"); return PRINT; }
"read"      { yylval.string = strdup("Read"); return READ; }

{alpha}{alphanum_us}*{alphanum}+|{alpha}    { yylval.string = strdup(yytext); return NAME; }
{num}+      { yylval.ival = atoi(yytext); return NUM; }
%%

int yywrap(){
	return 1;
}