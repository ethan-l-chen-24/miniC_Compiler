%{
#include <stdio.h>
//#include "./ast/ast.h"
void yyerror(const char *);
extern int yylex();
extern int yylex_destroy();
extern FILE *yyin;
extern int yylineno;
extern char* yytext;
%}

%union {
	int ival;
	char* string;
	//astNode* node;
}

%token EXTERN INT VOID RETURN IF ELSE WHILE LT GT LE GE EQ NE 
%token<ival> NUM
%token<string> NAME READ PRINT
//%type<astNode> minic extern func_def assign_statement expression
//%type<string> func_header

%nonassoc IF
%nonassoc ELSE

%start minic
%%
minic:
	extern extern func_def 
	;

extern:
	EXTERN VOID PRINT '(' INT ')' ';' |
	EXTERN INT READ '(' ')' ';' 
	;

func_def:
	func_header block_statement
	;

type:
	INT |
	VOID
	;

term:
	NAME |
	NUM
	;

func_header:
	type NAME '(' type NAME ')' | 
	type NAME '(' ')'
	;

block_statement:
	'{' statements '}' |
	'{' var_decs statements '}'
	;

var_decs:
	var_decs declaration |
	declaration
	;

declaration:
	INT NAME ';';

statements:
	statements statement |
	statement
	;

statement:
	assign_statement |
	if_statement |
	while_loop |
	block_statement |
	call_statement |
	return_statement
	;

assign_statement:
	NAME '=' expression ';' |
	NAME '=' term ';' |
	NAME '=' READ '(' ')' ';'
	;

call_statement:
	PRINT '(' expression ')' ';' |
	PRINT '(' term ')' ';' 
	;

return_statement:
	RETURN term ';' ;

expression:
	term '+' term |
	term '-' term |
	term '/' term |
	term '*' term |
	'-' term 
	;

condition:
	'(' term comp term ')' 
	;

comp:
	LT | 
	GT | 
	LE | 
	GE | 
	EQ | 
	NE
	;

if_statement:
	IF condition statement |
	IF condition statement ELSE statement 
	;  


while_loop:
	WHILE condition statement
	;

%%

int main(int argc, char** argv){
	if (argc == 2){
		yyin = fopen(argv[1], "r");
	}

	yyparse();

	if (yyin != stdin)
		fclose(yyin);

	yylex_destroy();
	
	return 0;
}


void yyerror(const char *){
	fprintf(stdout, "Syntax error %d\n", yylineno);
}
