%{
#include <stdio.h>
#include "../lib/ast/ast.h"
void yyerror(const char *);
extern int yylineno;
extern int yylex();

extern astNode* root;
%}

%union {
	int ival;
	char* string;
	astNode* node;	
	vector<astNode*>* stmt_vec;
}

%token EXTERN INT VOID RETURN IF ELSE WHILE LT GT LE GE EQ NEQ
%token<ival> NUM
%token<string> NAME READ PRINT
%type<node> minic extern func_def
%type<node> term expression condition 
%type<node> statement assign_statement if_statement while_loop call_statement return_statement block_statement declaration
%type<stmt_vec> statements var_decs

%nonassoc IF
%nonassoc ELSE

%start minic
%%
minic:
	extern extern func_def { $$ = createProg($1, $2, $3); root = $$; }
	;

extern:
	EXTERN VOID PRINT '(' INT ')' ';' { $$ = createExtern($3); free($3); } |
	EXTERN INT READ '(' ')' ';' { $$ = createExtern($3); free($3); }
	;

func_def:
	INT NAME '(' INT NAME ')' block_statement { $$ = createFunc($2, createVar($5), $7); free($2); free($5); } | 
	INT NAME '(' ')' block_statement { $$ = createFunc($2, NULL, $5); free($2); }
	;

term:
	NAME { $$ = createVar($1); free($1); } |
	NUM { $$ = createCnst($1); }
	;

block_statement:
	'{' statements '}' { $$ = createBlock($2); } |
	'{' var_decs statements '}' { 
			$2->insert($2->end(), $3->begin(), $3->end()); 
			delete($3); 
			$$ = createBlock($2); 
		}
	;

var_decs:
	var_decs declaration { $$ = $1; $$->push_back($2); } |
	declaration { $$ = new vector<astNode*>(); $$->push_back($1); }
	;

declaration:
	INT NAME ';' { $$ = createDecl($2); free($2); } 
	;

statements:
	statements statement { $$ = $1; $$->push_back($2); } |
	statement { $$ = new vector<astNode*>(); $$->push_back($1); }
	;

statement:
	assign_statement { $$ = $1; } |
	if_statement { $$ = $1; } |
	while_loop { $$ = $1; } |
	block_statement { $$ = $1; } |
	call_statement { $$ = $1; } |
	return_statement { $$ = $1; } 
	;

assign_statement:
	NAME '=' expression ';' { $$ = createAsgn(createVar($1), $3); free($1); } |
	NAME '=' term ';' { $$ = createAsgn(createVar($1), $3); free($1); } |
	NAME '=' READ '(' ')' ';' { $$ = createAsgn(createVar($1), createCall($3)); free($1); free($3); }
	;

call_statement:
	PRINT '(' expression ')' ';' { $$ = createCall($1, $3); free($1); } |
	PRINT '(' term ')' ';' { $$ = createCall($1, $3); free($1); } 
	;

return_statement:
	RETURN term ';' { $$ = createRet($2); } |
	RETURN expression ';' { $$ = createRet($2); } |
	RETURN READ '(' ')' ';' { $$ = createRet(createCall($2)); free($2); }
	;

expression:
	term '+' term { $$ = createBExpr($1, $3, add); } |
	term '-' term { $$ = createBExpr($1, $3, sub); } |
	term '/' term { $$ = createBExpr($1, $3, divide); } |
	term '*' term { $$ = createBExpr($1, $3, mul); } |
	'-' term { $$ = createUExpr($2, uminus); } 
	;

condition:
	'(' term LT term ')' { $$ = createRExpr($2, $4, lt); } |
	'(' term GT term ')' { $$ = createRExpr($2, $4, gt); } |
	'(' term LE term ')' { $$ = createRExpr($2, $4, le); } |
	'(' term GE term ')' { $$ = createRExpr($2, $4, ge); } |
	'(' term EQ term ')' { $$ = createRExpr($2, $4, eq); } |
	'(' term NEQ term ')' { $$ = createRExpr($2, $4, neq); }
	;

if_statement:
	IF condition statement { $$ = createIf($2, $3); } |
	IF condition statement ELSE statement { $$ = createIf($2, $3, $5); }
	;  


while_loop:
	WHILE condition statement { $$ = createWhile($2, $3); }
	;

%%

void yyerror(const char *){
	fprintf(stdout, "Syntax error %d\n", yylineno);
}
