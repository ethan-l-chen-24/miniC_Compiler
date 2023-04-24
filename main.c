/* 
 * This is the main program for the miniC compiler, which links together all of the individual compiler components.
 * It takes a single input with a miniC file, which then tokenizes, analyzes, optimizes and turns into machine code
 * to be executed.
*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include "syntax_analyzer/semantic_analysis.h"
using namespace std;

/* EXTERNS */
/* ------- */

extern void yyparse();
extern int yylex_destroy();
extern FILE *yyin;
extern char* yytext;

/* GLOBAL VARS */
/* ----------- */

astNode* root; // root of the AST


/* FUNCTION PROTOTYPES */
/* ------------------- */
void endProgram();

/* MAIN */
/* ------- */

int main(int argc, char** argv){
	if (argc == 2){
		yyin = fopen(argv[1], "r");
	}

    // generate the AST
	yyparse();

    // check semantics of the program
    bool valid_semantics = semanticAnalysis_opt(root); // run with or without _opt extension - _opt trades memory for runtime

    if(!valid_semantics) {
        endProgram();
        return 1;
    }

    // convert to LLVM IR

    // Optimize the LLVM IR

    // Convert to machine code

    endProgram();
	return 0;
}

void endProgram() {
    if (yyin != stdin)
		fclose(yyin);
	yylex_destroy();
    freeNode(root);
}
