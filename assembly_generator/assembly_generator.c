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
#include "../optimizer/llvm_optimizations.h"
#include "../helper/helper_functions.h"
#include "llvm_to_assembly.h"
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
	
	if (argc >= 3){
		yyin = fopen(argv[2], "r");
	}

	LLVMModuleRef llvm_ir;
	if(strcmp("build", argv[1]) == 0) {
		// generate the AST
		yyparse();

		// generate LLVM
		llvm_ir = createLLVMModelFromAST(root, argv[2]);
		optimizeLLVMBasicBlocks(llvm_ir);

		// add optimizations here
		optimizeLLVM(llvm_ir);

		// generate assembly
		codegen(llvm_ir, argv[3]);

		freeNode(root);

	} else {
		llvm_ir = createLLVMModel(argv[2]);

		// add optimizations here
		optimizeLLVM(llvm_ir);

		// generate assembly
		codegen(llvm_ir, argv[3]);
	}

	// close
	if (yyin != stdin) {
		fclose(yyin);
		yylex_destroy();
	}
	
	return 0;
}