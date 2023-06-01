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
#include "llvm_ir_builder/llvm_gen.h"
#include "optimizer/llvm_optimizations.h"
#include "helper/helper_functions.h"
#include "assembly_generator/llvm_to_assembly.h"

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
	if (argc == 3){
		yyin = fopen(argv[1], "r");
	} else {
        fprintf(stderr, "Please run the file as so: ./[source].out [input filepath] [output filepath]");
        return 1;
    }

    // generate the AST
	yyparse();
    printf("SUCCESS: AST Generated\n");

    // check semantics of the program
    bool valid_semantics = semanticAnalysis_opt(root); // run with or without _opt extension - _opt trades memory for runtime

    if(!valid_semantics) {
        freeNode(root);
        printf("FAILURE: Semantics Failed\n");
        return 1;
    }
    printf("SUCCESS: Semantics Checked\n");

    // convert to LLVM IR
    LLVMModuleRef llvm_ir = createLLVMModelFromAST(root, argv[1]);
    optimizeLLVMBasicBlocks(llvm_ir);
    printf("SUCCESS: LLVM IR Built\n");

    freeNode(root);

    // Optimize the LLVM IR
    optimizeLLVM(llvm_ir);
    printf("SUCCESS: LLVM IR Optimized\n");

    // Convert to machine code
    codegen(llvm_ir, argv[2]);
    printf("SUCCESS: Assembly Generated\n");

   // endProgram();
    printf("Program ended");

	return 0;
}

void endProgram() {
    if (yyin != stdin) {
		fclose(yyin);
	    yylex_destroy();
    }
}