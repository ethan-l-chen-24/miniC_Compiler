/*
 * Library that contains methods to optimize a LLVM IR module
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_optimizations.h"
#include <unordered_map>
#define NDEBUG
#include <cassert>

using namespace std;

/* FUNCTION PROTOTYPES */
bool runLocalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMBasicBlockRef bb));
bool runGlobalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMValueRef func));
bool eraseInstructions(vector<LLVMValueRef>* instructions);

/* FUNCTIONS */
/* --------- */

/* runs all of the necessary optimizations on the LLVMModule such that
 * as many unnecessary instructions as possible are eliminated
 */
void optimizeLLVM(LLVMModuleRef mod) {
    assert(mod != NULL);

    runLocalOptimizations(mod, commonSubexpressionElimination);
    runLocalOptimizations(mod, deadCodeElimination);

    // run constant folding and constant propagation until no more changes
    bool changed = true;
    while(changed) {
        changed = false;
        changed |= runLocalOptimizations(mod, constantFolding);
        changed |= runGlobalOptimizations(mod, constantPropagation);
    }

}

/* iterates through all of the basic blocks in each function
 * runs the specified function and returns true if changes have been made
 */
bool runLocalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMBasicBlockRef bb)) {
    assert(mod != NULL);
    assert(opt != NULL);

    bool changed = false; // bool to track if any changes have been made to instructions

    // loop through functions
    for(LLVMValueRef function =  LLVMGetFirstFunction(mod); 
        function; 
        function = LLVMGetNextFunction(function)) {

        for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
            basicBlock;
            basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
                // run the passed optimization
                changed |= (*opt)(basicBlock);
            }
    }

    return changed;
}

/* iterates through all of the basic blocks in each function
 * runs the specified function and returns true if changes have been made
 */
bool runGlobalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMValueRef func)) {
    assert(mod != NULL);
    assert(opt != NULL);

    bool changed = false; // bool to track if any changes have been made to instructions

    // loop through functions
    for(LLVMValueRef function =  LLVMGetFirstFunction(mod); 
        function; 
        function = LLVMGetNextFunction(function)) {

        // run the passed optimization
        changed |= (*opt)(function);
    }

    return changed;
}

/* Finds pairs of instructions with the same opcode, and operands
 * If there is no modifier (store when opcode is load) of the operands between these instructions
 * then replace all uses of the second instruction with the first
 */
bool commonSubexpressionElimination(LLVMBasicBlockRef bb) {
    assert(bb != NULL);

    unordered_map<LLVMOpcode, vector<LLVMValueRef>> instructionsByOp; // from opcode to instruction
    unordered_map<LLVMValueRef, LLVMValueRef> activeLoadInstructions; // from loaded var to load instruction
    vector<LLVMValueRef>* instructionsToErase = new vector<LLVMValueRef>(); // instructions to erase

    // walk instructions
    int k = 0;
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        k++;
        // retrieve the opCode
        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

        // handle alloca case (don't do anything)
        if(opcode == LLVMAlloca) {
            continue;
        }

        // handle store case (eliminate loads)
        if(opcode == LLVMStore) {
            assert(LLVMGetNumOperands(instruction) == 2);
            LLVMValueRef addr = LLVMGetOperand(instruction, 1);

            // if there is a load on the addr, erase it from the active loads
            if(activeLoadInstructions.count(addr)) {
                activeLoadInstructions.erase(addr);
            }
            continue;
        }

        // handle load case (check for common uses)
        if(opcode == LLVMLoad) { // load instruction
            assert(LLVMGetNumOperands(instruction) == 1);
            LLVMValueRef addr = LLVMGetOperand(instruction, 0);

            // if there is a load, use it
            if(activeLoadInstructions.count(addr)) {
                LLVMReplaceAllUsesWith(instruction, activeLoadInstructions[addr]);
                instructionsToErase->push_back(instruction);
            } else {
                activeLoadInstructions[addr] = instruction;
            }
            continue;
        }

        // check for common subexpressions

        // if it is the first instruction of its opcode
        if(!instructionsByOp.count(opcode)) {
            vector<LLVMValueRef> instructions;
            instructions.push_back(instruction);
            instructionsByOp[opcode] = instructions;
            continue;
        }

        // otherwise, loop through the instructions and look for common operators
        vector<LLVMValueRef> instructions = instructionsByOp[opcode];
        vector<LLVMValueRef>::iterator it = instructions.begin();
        while(it != instructions.end()) {
            assert(*it != NULL);
            assert(LLVMGetInstructionOpcode(instruction) == LLVMGetInstructionOpcode(*it));
            assert(LLVMGetNumOperands(instruction) == LLVMGetNumOperands(*it));

            // check that all the operands are the same
            bool sameOperands = true;
            for(int i = 0; i < LLVMGetNumOperands(instruction); i++) {
                if(LLVMGetOperand(instruction, i) != LLVMGetOperand(*it, i)) {
                    sameOperands = false;
                    break;
                }
            }

            // if all the operands are the same, replace the uses
            if(sameOperands) {
                LLVMReplaceAllUsesWith(instruction, *it);
                instructionsToErase->push_back(instruction);
                break;
            }

            it++;
        }
    }

    return eraseInstructions(instructionsToErase);
}

/* Finds instructions that have no use
 * these instructions will follow a return or branch
 */
bool deadCodeElimination(LLVMBasicBlockRef bb) {
    assert(bb != NULL);

    bool terminatorReached = false;
    vector<LLVMValueRef>* instructionsToErase = new vector<LLVMValueRef>(); // instructions to erase

    // walk instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        // if we have hit a branch or return, delete deadcode
        if(terminatorReached) {
            instructionsToErase->push_back(instruction);
        }

        // retrieve the opCode
        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

        // if we have a branch or return, enable flag to delete deadcode
        if(opcode == LLVMRet || opcode == LLVMBr) {
            terminatorReached = true;
        }

    }

    return eraseInstructions(instructionsToErase);
}

/* Finds instructions with opcode +, -, * and all operands are constants
 * folds the operation into a single constant
 */
bool constantFolding(LLVMBasicBlockRef bb) {
    assert(bb != NULL);

    vector<LLVMValueRef>* instructionsToErase = new vector<LLVMValueRef>(); // instructions to erase

    // walk instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
        instruction;
  		instruction = LLVMGetNextInstruction(instruction)) {

        // retrieve the opCode
        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

        // if we don't have a binary operator continue
        if(!LLVMIsABinaryOperator(instruction)) {
            continue;
        }

        // get the operands
        assert(LLVMGetNumOperands(instruction) == 2);
        LLVMValueRef op1 = LLVMGetOperand(instruction, 0);
        LLVMValueRef op2 = LLVMGetOperand(instruction, 1);

        // check that they are constants
        if(!LLVMIsAConstantInt(op1) || !LLVMIsAConstantInt(op2)) {
            continue;
        }

        // fold the constants
        switch(opcode) {
            case(LLVMAdd): {
                LLVMValueRef add = LLVMConstAdd(op1, op2);
                LLVMReplaceAllUsesWith(instruction, add);
                instructionsToErase->push_back(instruction);
                break;
            }

            case(LLVMSub): {
                LLVMValueRef sub = LLVMConstSub(op1, op2);
                LLVMReplaceAllUsesWith(instruction, sub);
                instructionsToErase->push_back(instruction);
                break;
            }

            case(LLVMMul): {
                LLVMValueRef mul = LLVMConstMul(op1, op2);
                LLVMReplaceAllUsesWith(instruction, mul);
                instructionsToErase->push_back(instruction);
                break;
            }

            default: {} // ignore div
        }

    }

    return eraseInstructions(instructionsToErase);
}

/*
 *
 */
bool constantPropagation(LLVMValueRef func) {
    return false;

}
/* erases all of the instructions in the list */
bool eraseInstructions(vector<LLVMValueRef>* instructions) {

    // return false if no instructions to delete
    if(instructions->size() == 0) {
        return false;
    } else { 

        // loop through the instructions and delete them
        vector<LLVMValueRef>::iterator it = instructions->begin();
        while(it != instructions->end()) {
            assert(*it != NULL);
            LLVMInstructionEraseFromParent(*it);
            it++;
        }

        // delete the list
        delete(instructions);

        return true;
    }
}