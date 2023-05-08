/* 
 * This is a test program for the llvm ir builder which takes the AST from the syntax analyzer
 * and gives the LLVM IR representation of the program
*/

#include<stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../syntax_analyzer/semantic_analysis.h"
#include "../llvm_ir_builder/llvm_gen.h"
#include "llvm_optimizations.h"
using namespace std;

/* EXTERNS */
/* ------- */

extern void yyparse();
extern int yylex_destroy();
extern FILE *yyin;
extern char* yytext;

/* FUNCTION PROTOTYPES */
/* ------------------- */


/* GLOBAL VARIABLES */
/* ---------------- */

astNode* root; // root of the AST

/* METHODS */
/* ------- */

/* Taken from 
 *
*/
LLVMModuleRef createLLVMModel(char * filename) {
	char *err = 0;

	LLVMMemoryBufferRef ll_f = 0;
	LLVMModuleRef m = 0;

	LLVMCreateMemoryBufferWithContentsOfFile(filename, &ll_f, &err);

	if (err != NULL) { 
		printf("%s", err);
		return NULL;
	}
	
	LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

	if (err != NULL) {
		printf("%s", err);
	}

	return m;
}

/* MAIN */
/* ---- */

int main(int argc, char** argv){
	
	if (argc >= 3){
		yyin = fopen(argv[2], "r");
	}

	LLVMModuleRef llvm_ir;
	if(strcmp("build", argv[1]) == 0) {
		// generate the AST
		yyparse();
		llvm_ir = createLLVMModelFromAST(root);

		// add optimizations here
		optimizeLLVM(llvm_ir);

		if(argc == 4) {
    		LLVMPrintModuleToFile(llvm_ir, argv[3], NULL);
		}
		yylex_destroy();
		freeNode(root);

	} else {
		llvm_ir = createLLVMModel(argv[2]);

		// add optimizations here
		optimizeLLVM(llvm_ir);

		if(argc == 4) {
    		LLVMPrintModuleToFile(llvm_ir, argv[3], NULL);
		}
	}

	// close
	if (yyin != stdin)
		fclose(yyin);
	
	return 0;
}