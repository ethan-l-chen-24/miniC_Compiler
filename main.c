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
    printf("SUCCESS: AST Generated\n");

    // check semantics of the program
    bool valid_semantics = semanticAnalysis_opt(root); // run with or without _opt extension - _opt trades memory for runtime

    if(!valid_semantics) {
        endProgram();
        printf("FAILURE: Semantics Failed\n");
        return 1;
    }
    printf("SUCCESS: Semantics Checked\n");

    // convert to LLVM IR
    LLVMModuleRef llvm_ir = createLLVMModelFromAST(root);
    printf("HERE");

    // Optimize the LLVM IR
    optimizeLLVM(llvm_ir);

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
