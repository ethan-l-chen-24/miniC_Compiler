/*
 * Library that contains methods to optimize a LLVM IR module
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_optimizations.h"
#include <map>

using namespace std;

/* FUNCTION PROTOTYPES */
bool runLocalOptimizations(LLVMModuleRef mod, void (*opt)(LLVMBasicBlockRef bb));
bool runGlobalOptimizations(LLVmModuleRef mod, void (*opt)(LLVMValueRef func));

/* FUNCTIONS */
/* --------- */

/* runs all of the necessary optimizations on the LLVMModule such that
 * as many unnecessary instructions as possible are eliminated
 */
void optimizeLLVM(LLVMModuleRef mod) {

    runLocalOptimizations(mod, commonSubexpressionElimination);
    runLocalOptimizations(mod, deadCodeElimination);

    // run constant folding and constant propagation until no more changes
    bool changed = true;
    while(changed) {
        changed = false;
        changed |= runLocalOptimizations(constantFolding);
        changed |= runGlobalOptimizations(constantPropagation);
    }

}

/* iterates through all of the basic blocks in each function
 * runs the specified function and returns true if changes have been made
 */
bool runLocalOptimizations(LLVMModuleRef mod, void (*opt)(LLVMBasicBlockRef bb)) {
    bool changed = false; // bool to track if any changes have been made to instructions

    // loop through functions
    for(LLVMValueRef function =  LLVMGetFirstFunction(module); 
        function; 
        function = LLVMGetNextFunction(function)) {

        // run the passed optimization
        changed |= (bool) (*(opt)(function));
    }

    return changed;
}

/* Finds pairs of instructions with the same opcode, and operands
 * If there is no modifier (store when opcode is load) of the operands between these instructions
 * then replace all uses of the second instruction with the first
 */
bool commonSubexpressionElimination(LLVMBasicBlockRef bb) {
    bool changed = false;

    // walk instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        

    }

    return changed;
}

/* Finds instructions that have no use
 * these instructions will follow a return or branch
 */
bool deadCodeElimination(LLVMBasicBlockRef bb) {
    bool changed = false;

    // walk instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        

    }

    return changed;
}

/* Finds instructions with opcode +, -, * and all operands are constants
 * folds the operation into a single constant
 */
bool constantFolding(LLVMBasicBlockRef bb) {
    bool changed = false;

    // walk instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        

    }

    return changed;
}

/*
 *
 */
bool constantPropagation(LLVMValueRef func) {

}