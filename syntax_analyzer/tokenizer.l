%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}

%option yylineno
alpha	[a-zA-Z]
alphanum [a-zA-Z0-9]
alphanum_us 	[a-zA-Z0-9_]
num     [0-9]

%%
";"     { printf("%s", yytext); return yytext[0]; }
"="     { printf("%s", yytext);  return yytext[0]; }

(\+|\-|\/|\*)    { printf("%s", yytext);  return yytext[0]; }
(\(|\)|\{|\})    { printf("%s", yytext);  return yytext[0]; }
"<"     { printf("LT"); return LT; }
">"     { printf("GT"); return GT; }
"<="    { printf("LE"); return LE; }
">="    { printf("GE"); return GE; }
"=="    { printf("EQ"); return EQ; }
"!="    { printf("NE"); return NE; }

"extern"    { printf("EXTERN"); return EXTERN; }
"int"   { printf("INT"); return INT; }
"void"      { printf("VOID"); return VOID; }
"return"    { printf("RETURN"); return RETURN; }

"if"    { printf("IF"); return IF; }
"else"  { printf("ELSE"); return ELSE; }
"while" { printf("WHILE"); return WHILE; }

"print"     { printf("PRINT"); yylval.string = strdup("Print"); return PRINT; }
"read"      { printf("READ"); yylval.string = strdup("Print"); return READ; }

{alpha}{alphanum_us}*{alphanum}+|{alpha}    { printf("%s", yytext); yylval.string = strdup(yytext); return NAME; }
{num}+      { printf("%s", yytext);  yylval.ival = atoi(yytext); return NUM; }
%%

int yywrap(){
	return 1;
}