%{
#include <stdio.h>
void yyerror(const char *);
extern int yylex();
extern int yylex_destroy();
extern FILE *yyin;
extern int yylineno;
extern char* yytext;
%}

%token COMP EXTERN INT VOID RETURN IF ELSE WHILE PRINT READ NAME NUM
%left '+' '-'
%left '*' '/'
%left ')'

%start MINIC

%%
MINIC:
	MINIC PRINT_FUNC | 
	MINIC READ_FUNC | 
	MINIC FUNC_DEC | 
	PRINT_FUNC | 
	READ_FUNC | 
	FUNC_DEC

PRINT_FUNC: 
	EXTERN VOID PRINT '(' INT ')' ';'

READ_FUNC:
	EXTERN INT READ '(' ')' ';'

PRINT_CALL:
	PRINT '(' EXPRESSION ')'

READ_CALL:
	READ '(' ')'

FUNC_DEC:
	INT NAME '(' INT NAME ')' '{' BODY '}' |
	INT NAME '(' ')' '{' BODY '}'

VAR_DEC:
	INT NAME

VAR_INIT:
	NAME '=' EXPRESSION

EXPRESSION:
	EXPRESSION '+' EXPRESSION |
	EXPRESSION '-' EXPRESSION |
	EXPRESSION '/' EXPRESSION |
	EXPRESSION '*' EXPRESSION |
	'-' EXPRESSION |
	'(' EXPRESSION ')' |
	NAME |
	NUM |
	READ_CALL

COMPARE:
	EXPRESSION COMP EXPRESSION

BODY:
	BODY VAR_DEC ';' | 
	BODY VAR_INIT ';' |
	BODY IF_STATEMENT |
	BODY WHILE_STATEMENT |
	BODY RETURN_STATEMENT ';' |
	BODY PRINT_CALL ';' |
	BODY READ_CALL ';' |

IF_STATEMENT:
	IF '(' COMPARE ')' '{' BODY '}' |
	IF '(' COMPARE ')' '{' BODY '}' ELSE '{' BODY '}'

WHILE_STATEMENT:
	WHILE '(' COMPARE ')' '{' BODY '}'

RETURN_STATEMENT:
	RETURN EXPRESSION

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
