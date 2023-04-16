%{
#include <stdio.h>
#include <string.h>
%}

%option yylineno
alpha	[a-zA-Z]
alphanum [a-zA-Z0-9]
alphanum_us 	[a-zA-Z0-9_]
num     [0-9]

%%
";"     { printf("%s", yytext); }
"="     { printf("%s", yytext); }

(\+|\-|\/|\*)    { printf("%s", yytext); }
(\(|\)|\{|\})    { printf("%s", yytext); }
"<"     { printf("LT"); }
">"     { printf("GT"); }
"<="    { printf("LE"); }
">="    { printf("GE"); }
"=="    { printf("EQ"); }
"!="    { printf("NE"); }

"extern"    { printf("EXTERN"); }
"int"   { printf("INT"); }
"void"      { printf("VOID"); }
"return"    { printf("RETURN"); }

"if"    { printf("IF"); }
"else"  { printf("ELSE"); }
"while" { printf("WHILE"); }

"print"     { printf("PRINT"); }
"read"      { printf("READ"); }

{alpha}{alphanum_us}*{alphanum}+|{alpha}    { printf("%s", yytext); }
{num}+      { printf("%s", yytext); }
%%

int yywrap(){
	return 1;
}

int main() {
    yylex();
    return(0);
}