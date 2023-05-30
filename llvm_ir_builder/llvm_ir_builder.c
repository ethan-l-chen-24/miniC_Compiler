/* 
 * This is a test program for the llvm ir builder which takes the AST from the syntax analyzer
 * and gives the LLVM IR representation of the program
*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../syntax_analyzer/semantic_analysis.h"
#include "llvm_gen.h"
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
	
	if (argc >= 2){
		yyin = fopen(argv[1], "r");
	}

    // generate the AST
	yyparse();

    LLVMModuleRef llvm_ir = createLLVMModelFromAST(root, argv[1]);
	optimizeLLVMBasicBlocks(llvm_ir);

	if(argc == 3) {
    	LLVMPrintModuleToFile(llvm_ir, argv[2], NULL);
	}

    // close
	if (yyin != stdin)
		fclose(yyin);
	yylex_destroy();
    freeNode(root);
	
	return 0;
}