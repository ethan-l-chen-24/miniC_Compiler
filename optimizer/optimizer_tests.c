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

/* FUNCTION PROTOTYPES */
/* ------------------- */
LLVMModuleRef testModule1();
LLVMModuleRef testModule2();
LLVMModuleRef testModule3();


/* MAIN */
/* ---- */

int main(int argc, char** argv){


	LLVMModuleRef llvm_ir = testModule3();

    // add optimizations here
	optimizeLLVM(llvm_ir);

	if(argc == 2) {
    	LLVMPrintModuleToFile(llvm_ir, argv[1], NULL);
	}
	
	return 0;
}

/* test module that tests:
 * deadcode elimination
 */
LLVMModuleRef testModule1() {
    //Creating a module 
    LLVMModuleRef mod = LLVMModuleCreateWithName("");
    LLVMSetTarget(mod, "x86_64-pc-linux-gnu");

    //Creating a function with a module
    LLVMTypeRef param_types[] = {};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 0, 0);
    LLVMValueRef func = LLVMAddFunction(mod, "test", ret_type);

    //Creating a basic block
    LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");

    //All instructions need to be created using a builder. The builder specifies
    //where the instructions are added.
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);

    //Creating an alloc instruction and assignment
    LLVMValueRef m = LLVMBuildAlloca(builder, LLVMInt32Type(), "m"); 
    LLVMValueRef n = LLVMBuildAlloca(builder, LLVMInt32Type(), "n"); 
    LLVMSetAlignment(m, 4);
    LLVMValueRef val = LLVMConstInt(LLVMInt32Type(), 10, false);
    LLVMBuildStore(builder, val, m);

    //use n
    LLVMValueRef temp = LLVMBuildLoad2(builder, LLVMInt32Type(), m, "");
    LLVMValueRef num = LLVMConstInt(LLVMInt32Type(), 12, false);
    LLVMValueRef add = LLVMBuildAdd(builder, temp, num, "");
    LLVMBuildStore(builder, add, n);

    // n has no use - should delete EVERYTHING except allocs

    return mod;
	
}

/* test module that tests:
 * common subexpression
 */
LLVMModuleRef testModule2() {
    //Creating a module 
    LLVMModuleRef mod = LLVMModuleCreateWithName("");
    LLVMSetTarget(mod, "x86_64-pc-linux-gnu");

    //Creating a function with a module
    LLVMTypeRef param_types[] = {};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 0, 0);
    LLVMValueRef func = LLVMAddFunction(mod, "test", ret_type);

    //Creating a basic block
    LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");

    //All instructions need to be created using a builder. The builder specifies
    //where the instructions are added.
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);

    //Creating an alloc instruction and assignment
    LLVMValueRef m = LLVMBuildAlloca(builder, LLVMInt32Type(), "m"); 
    LLVMValueRef n = LLVMBuildAlloca(builder, LLVMInt32Type(), "n"); 
    LLVMSetAlignment(m, 4);
    LLVMSetAlignment(n, 4);

    //Build a common add statement
    LLVMValueRef temp1 = LLVMBuildLoad2(builder, LLVMInt32Type(), m, "");
    LLVMValueRef num1 = LLVMConstInt(LLVMInt32Type(), 12, false);
    LLVMValueRef add1 = LLVMBuildAdd(builder, temp1, num1, "");
    LLVMBuildStore(builder, add1, n);

    LLVMValueRef temp2 = LLVMBuildLoad2(builder, LLVMInt32Type(), m, "");
    LLVMValueRef num2 = LLVMConstInt(LLVMInt32Type(), 12, false);
    LLVMValueRef add2 = LLVMBuildAdd(builder, temp2, num2, "");
    LLVMBuildStore(builder, add2, m);

    //Build a load (should not delete this one since m was stored)
    LLVMValueRef mChanged = LLVMBuildLoad2(builder, LLVMInt32Type(), m, "");

    LLVMBuildRet(builder, mChanged);

    // one add should be deleted and one load should be deleted

    return mod;
}

/* test module that tests:
 * constant folding
 */
LLVMModuleRef testModule3() {
    //Creating a module 
    LLVMModuleRef mod = LLVMModuleCreateWithName("");
    LLVMSetTarget(mod, "x86_64-pc-linux-gnu");

    //Creating a function with a module
    LLVMTypeRef param_types[] = {};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 0, 0);
    LLVMValueRef func = LLVMAddFunction(mod, "test", ret_type);

    //Creating a basic block
    LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");

    //All instructions need to be created using a builder. The builder specifies
    //where the instructions are added.
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);

    //Creating an alloc instruction and assignment
    LLVMValueRef m = LLVMBuildAlloca(builder, LLVMInt32Type(), "m"); 
    LLVMSetAlignment(m, 4);

    LLVMValueRef num1 = LLVMBuildInt(LLVMInt32Type(), 12, false);
    LLVMValueRef num2 = LLVMConstInt(LLVMInt32Type(), 15, false);
    LLVMValueRef add1 = LLVMBuildAdd(builder, num1, num2, "");
    LLVMBuildStore(builder, add1, m);

    LLVMValueRef out = LLVMBuildLoad2(builder, LLVMInt32Type(), m, "");
    LLVMBuildRet(builder, out);

    // the add statement should be folded

    return mod;
}

