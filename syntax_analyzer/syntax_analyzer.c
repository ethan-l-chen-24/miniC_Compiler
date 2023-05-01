/* 
 * This is a test program for the syntax analyzer which tokenizes and analyzes the code, printing out the AST
 * and returning if the program is semantically sound
*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include "semantic_analysis.h"
using namespace std;

/* EXTERNS */
/* ------- */

extern void yyparse();
extern int yylex_destroy();
extern FILE *yyin;
extern char* yytext;

/* GLOBAL VARIABLES */
/* ---------------- */

astNode* root; // root of the AST

/* MAIN */
/* ---- */

int main(int argc, char** argv){
	if (argc == 2){
		yyin = fopen(argv[1], "r");
	}

    // generate the AST
	yyparse();
    printf("AST:\n--------------\n");
    printNode(root);
    printf("\nRESULT:\n---------\n");

    // check semantics of the program
    bool validSemantics = semanticAnalysis_opt(root); // run with or without _opt extension - _opt trades memory for runtime
    freeNode(root);

    // close
	if (yyin != stdin)
		fclose(yyin);
	yylex_destroy();
	
    // return if failure
    if(!validSemantics) return 1;

    printf("Looks good to me!\n\n");
	return 0;
}