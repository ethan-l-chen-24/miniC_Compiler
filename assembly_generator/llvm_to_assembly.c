/*
 * Library that contains methods to turn LLVM IR into assembly
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../optimizer/llvm_optimizations.h"
#include "llvm_to_assembly.h"
#include <unordered_map>
#include <vector>
#include <set>
#include <array>
#include <string>
#include <algorithm>
#include <iostream>
//#define NDEBUG
#include <cassert>

using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */

// methods to generate register map
void getIndices(LLVMBasicBlockRef bb);
void computeLiveness(LLVMBasicBlockRef bb);
void sortList();
void registerAllocation(LLVMValueRef func);
    LLVMValueRef findSpill();
    void releaseOperands(LLVMValueRef instruction, int index);

// other helper methods
void createBBLabels(LLVMValueRef func);
void printDirectives(LLVMValueRef func);
void printFunctionEnd(LLVMValueRef func);
void getOffsetMap(LLVMValueRef func);

// instruction specific helper methods
void handleRet(LLVMValueRef instruction);
void handleLoad(LLVMValueRef instruction);
void handleStore(LLVMValueRef instruction);
void handleCall(LLVMValueRef instruction);
void handleBranch(LLVMValueRef instruction);
void handleAlloca(LLVMValueRef instruction);
void handleArithmetic(LLVMValueRef instruction);


/* GLOBAL VARIABLES */
/* ---------------- */

unordered_map<LLVMValueRef, int> instIndex;
unordered_map<LLVMValueRef, array<int, 2>> liveRange;
vector<LLVMValueRef> sortedList;
unordered_map<LLVMValueRef, string> regMap;
vector<string> regPool;

unordered_map<LLVMBasicBlockRef, char*> bbLabels;
unordered_map<LLVMValueRef, int> offsetMap;
int localMem;

FILE *fptr;

/* FUNCTIONS */
/* --------- */

/* assigns an index to every instruction in the 
 * LLVM code
 */
void getIndices(LLVMBasicBlockRef bb) {
    assert(bb != NULL);

    int counter = 0;

    // loop through instructions
    for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
        instruction;
        instruction = LLVMGetNextInstruction(instruction)) {

        // assign the instruction an index value
        instIndex[instruction] = counter;
        counter++;
    }
}

/* computes the 'liveness' of each instruction
 * that is, from the line it is defined to its last use
 * as an operand
 */
void computeLiveness(LLVMBasicBlockRef bb) {
    assert(bb != NULL);

    for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
        instruction;
        instruction = LLVMGetNextInstruction(instruction)) {

        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);
            
        // skip alloca instructions or any that do not return some value
        if(opcode == LLVMAlloca) {
            continue;
        }

        // add begin index for the instruction
        if(liveRange.count(instruction) == 0) {
            array<int, 2> arr;
            arr[0] = instIndex[instruction];
            arr[1] = -1;
            liveRange[instruction] = arr;
        }

        // loop through operands and update end index
        for(int i = 0; i < LLVMGetNumOperands(instruction); i++) {
            LLVMValueRef operand = LLVMGetOperand(instruction, i);

            // skip if the operand doesn't have a solid instruction value
            if(liveRange.count(operand) == 0) {
                continue;
            }

            liveRange[operand][1] = instIndex[instruction];
        }
    }

    // flag all values that are never used again (instructions that don't save values)
    vector<LLVMValueRef> instructionsToErase;
    for(auto i : liveRange) {
        if(i.second[1] == -1) { // single use - never used as an operand
            instructionsToErase.push_back(i.first);
        }
    }

    // remove those instructions from the live range - don't need to be stored
    vector<LLVMValueRef>::iterator it = instructionsToErase.begin();
    while(it != instructionsToErase.end()) {
        liveRange.erase(*it);
        it++;
    }
}

/* sorts the list of instructions by end points in live_map */
void sortList() {
    // loop through the map and add all of the active instructions to the list
    for(auto i : liveRange) {
        sortedList.push_back(i.first);
    }

    // sort by end points in liveRange (decreasing order)
    sort(sortedList.begin(), sortedList.end(), [](LLVMValueRef a, LLVMValueRef b) {
        return (liveRange[a][1] >liveRange[b][1]);
    });
}

/* assigns each instruction (that needs one) a register
 * handles spill values in case we run out of registers
 * in which case registers that release values earlier are spilled
 */
void registerAllocation(LLVMValueRef func) {
    assert(func != NULL);

    // loop through basic blocks
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
        bb;
        bb = LLVMGetNextBasicBlock(bb)) {
        
        // add registers to register pool 
        regPool.clear();
        regPool.push_back("ebx");
        regPool.push_back("ecx");
        regPool.push_back("edx");

        // build register map and liveness map
        instIndex.clear();
        liveRange.clear();
        sortedList.clear();
        getIndices(bb);
        computeLiveness(bb);
        sortList();

        // loop through instructions
        int index = 0;
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);
            
            // skip alloca instructions or any that do not return some value that are used
            // this covers store, branch, return instructions, as well as allocas and calls with void return types
            if(liveRange.count(instruction) == 0) {
                releaseOperands(instruction, index);
                index++;
                continue;

            // if arithmetic, check for register transfer
            } else if(opcode == LLVMAdd || opcode == LLVMSub || opcode == LLVMMul) {
                assert(LLVMGetNumOperands(instruction) == 2);
                LLVMValueRef operand1 = LLVMGetOperand(instruction, 0);

                // check if the operand is assigned to a register
                if(regMap.count(operand1) > 0 && regMap[operand1] != "-1") {
                    assert(liveRange.count(operand1) != 0);

                    // if this is that operands last use, we can transfer the register
                    if(liveRange[operand1][1] == index) {
                        regMap[instruction] = regMap[operand1];
                    }
                }

                // check the second operand, and if it ends, return its register to register pool
                LLVMValueRef operand2 = LLVMGetOperand(instruction, 1);
                if(regMap.count(operand2) > 0 && regMap[operand2] != "-1") {
                    assert(liveRange.count(operand2) != 0);

                    // if this is that operands last use, we can transfer the register
                    if(liveRange[operand2][1] == index) {
                        regPool.push_back(regMap[operand2]);
                    }
                }

                index++;
                continue;
            } else 
            
            // in all other cases, just get a register or spill
            if(regPool.size() > 0) {
                // retrieve a register and assign it
                string reg = regPool.back();
                regPool.pop_back();
                regMap[instruction] = reg;

            } else {
                // get a value to spill
                LLVMValueRef spillVal = findSpill();
                assert(liveRange.count(spillVal) != 0 && liveRange.count(instruction) != 0);
                assert(regMap[spillVal] != "-1");

                // if end of instruction is after end of spill value
                if(liveRange[instruction][1] > liveRange[spillVal][1]) {
                    regMap[instruction] = regMap[spillVal];
                    regMap[spillVal] = "-1";
                }

            }

            // release operands
            releaseOperands(instruction, index);

            // increment instruction index
            index++;
        }

    }
}

/* returns a spill value (instruction with physical register) 
 * by end index, in decreasing order
 */
LLVMValueRef findSpill() {
    // loop through sorted list
    vector<LLVMValueRef>::iterator it = sortedList.begin();
    while(it != sortedList.end()) {
        // skip if not in reg map or -1
        if(regMap.count(*it) == 0 || regMap[*it] == "-1") {
            it++;
            continue;
        }

        return *it;
    }

    // no instructions match, return null
    return NULL;
}

/* loops through all of the operands of an instruction
 * if that operand ends at the current index, we release its register back
 * to the register pool (if it has one)
 */
void releaseOperands(LLVMValueRef instruction, int index) {
    // loop through operands
    for(int i = 0; i < LLVMGetNumOperands(instruction); i++) {
        LLVMValueRef operand = LLVMGetOperand(instruction, i);
        if(liveRange.count(operand) == 0) {
            continue;
        }

        // if live range of an operand ends, release register
        if(liveRange[operand][1] == index) {
            // if parameter not assigned a value 
            if(regMap.count(operand) == 0) {
                continue;
            }

            // only release register if the operand isn't spilled
            if(regMap[operand] != "-1") {
                regPool.push_back(regMap[operand]);
            } 
        }
    }
}

/* Generates the assembly code given a LLVM module
 * and outputs the assembly to the given filepath
 */
void codegen(LLVMModuleRef mod, char* filepath) {
    assert(mod != NULL);
    assert(filepath != NULL);

    fptr = fopen(filepath, "w");

    // loop through each function
    for(LLVMValueRef func = LLVMGetFirstFunction(mod);
        func;
        func = LLVMGetNextFunction(func)) {

        // allocate registers and populate global variables
        registerAllocation(func);
        printDirectives();
        getOffsetMap();

        // loop through basic blocks
        for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
            bb;
            bb = LLVMGetNextBasicBlock(bb)) {

            // loop through instructions
            for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
                instruction;
                instruction = LLVMGetNextInstruction(instruction)) {

                LLVMOpcode op = LLVMGetInstructionOpcode(instruction);

                // handle instruction types
                switch(op) {
                    case LLVMRet:
                        handleRet(instruction);
                        break;
                    case LLVMLoad:
                        handleLoad(instruction);
                        break;
                    case LLVMStore:
                        handleStore(instruction);
                        break;
                    case LLVMCall:
                        handleCall(instruction);
                        break;
                    case LLVMBr:
                        handleBranch(instruction);
                        break;
                    case LLVMAlloca:
                        handleAlloca(instruction);
                        break;
                    case LLVMAdd:
                        handleArithmetic(instruction);
                        break;
                    case LLVMSub:
                        handleArithmetic(instruction);
                        break;
                    case LLVMMul:
                        handleArithmetic(instruction);
                        break; 
                }
            }

        }

    }
}

/* assigns each basic block in the module a char* label
 * the first block will be assigned '.LFB0'
 * all subsequent blocks will be labeled '.[block index]'
 */
void createBBLabels(LLVMValueRef func) {

    // loop through basic blocks
    int i = 0;
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
        bb;
        bb = LLVMGetNextBasicBlock(bb)) {

        if(i == 0) {
            char firstLabel[] = ".LFB0";
            bbLabels[bb] = firstLabel;
        } else {
            char buf[256];
            char* label = ".";
            sprintf(buf, ".L%d", i);

            // concatenate index to it
            bbLabels[bb] = buf;
        }

        i++;
    }

}

/* prints the directives of the function */
void printDirectives(LLVMValueRef func) {

}

/* prints the instructions to close a function */
void printFunctionEnd(LLVMValueRef func) {

}

/* populates the offset map with allocas mapped 
 * to their offsets from %ebp
 * this method assumes that all allocas are placed at the beginning of the
 * first basic block
 */
void getOffsetMap(LLVMValueRef func) {

    // get first basic block that contains allocas
    LLVMBasicBlockRef firstBlock = LLVMGetFirstBasicBlock(func);
    assert(firstBlock != NULL);

    // get instructions
    LLVMValueRef allocaInstruction = LLVMGetFirstInstruction(firstBlock);
    int offset = 0;

    // loop through all allocas instructions
    while(allocaInstruction != NULL && LLVMGetOpcode(allocaInstruction) == LLVMAlloca) {
        offset += 4;
        offsetMap[allcaInstruction] = offset;
    }

    // set global local mem value
    localMem = offset;
}

/* handles return statements */
void handleRet(LLVMValueRef instruction, LLVMValueRef func) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMRet);
    assert(LLVMGetNumOperands(instruction) == 1);

    LLVMValueRef retVal = LLVMGetOperand(instruction, 0);
    if(LLVMIsAConstantInt(retVal)) {
        int val = LLVMConstIntGetSExtValue(retVal);
        fprintf(fptr, "movl %d, %%eax\n", val);
    } else if(regMap.count(retVal) != 0 && regMap[retVal] == "-1") {
        int offset = offsetMap[retVal];
        fprintf(fptr, "movl %d(%%ebp), %%eax\n", offset);
    } else if(regMap.count(retVal) != 0) {
        fprintf(fptr, "movl %s, %%eax\n", regMap[retVal].c_str());
    }

    printFunctionEnd(func); // TODO add func as parameter
}

/* handles load statements */
void handleLoad(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMLoad);
    assert(LLVMGetNumOperands(instruction) == 1);

    LLVMValueRef loadVal = LLVMGetOperand(instruction, 0);

    if(regMap.count(instruction) != 0 && regMap[instruction] != "-1") {
        assert(offsetMap.count(loadVal) != 0);
        int offset = offsetMap[retVal];
        fprintf(fptr, "movl %d(%%ebp), %s\n", offset, regMap[loadVal].c_str());
    }
}

/* handles store statements */
void handleStore(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMStore);
    assert(LLVMGetNumOperands(instruction) == 2);

    // handle if A is a parameter TODO
    LLVMValueRef storeVal = LLVMGetOperand(instruction, 0);
    LLVMValueRef destination = LLVMGetOperand(instruction, 1);
    assert(offsetMap.count(destination) != 0);

    if(LLVMIsAConstantInt(storeVal)) {
        int val = LLVMConstIntGetSExtValue(storeVal);
        int offset = offsetMap[destination];
        fprintf(fptr, "movl %d, %d(%%ebp)\n", val, offset);

    } else {
        assert(regMap.count(storeVal) != 0);

        if(regMap[storeVal] == "-1") {
            int valOffset = offsetMap[storeVal];
            int destinationOffset = offsetMap[destination];
            fprintf(fptr, "movl %d(%%ebp), %%eax\n", valOffset)
            fprintf(fptr, "movl %%eax, %d(%%ebp)\n", destinationOffset);
            
        } else {
            int offset = offsetMap[storeVal];
            fprintf(fptr, "movl %s, %d(%%ebp)\n", regMap[storeVal].c_str(), offset);
        }
    }
}

/* handles call statements */
void handleCall(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMCall);

    funcName = ""; // TODODODODO

    fprintf(fptr, "pushl %%ebx\n");
    fprintf(fptr, "pushl %%ecx\n");
    fprintf(fptr, "pushl %%edx\n");

    LLVMValueRef calledFunc = LLVMGetCalledValue(instruction);
    int numParams = LLVMCountParams(calledFunc);

    if(numParams == 1) {
        LLVMValueRef param = LLVMGetParam(calledFunc, 0);

        if(LLVMIsAConstantInt(param)) {
            int val = LLVMConstIntGetSExtValue(param);
            fprintf(fptr, "pushl $%d", val);
        } else {
            assert(regMap.count(param) != 0);

            if(regMap[param] == "-1") {
                assert(offsetMap.count(param) != 0);
                int offset = offsetMap[param];

                fprintf(fptr, "pushl %d(%%ebp)", offset);
            } else {
                fprintf(fptr, "pushl %s", regMap[param].c_str());
            }
        }
    }

    fprintf(fptr, "call %s", funcName.c_str());

    if(numParams == 1) {
        fprintf(fptr, "addl $4, %%esp");
    }

    fprintf(fptr, "popl %%ebx\n");
    fprintf(fptr, "popl %%ecx\n");
    fprintf(fptr, "popl %%edx\n");
}

/* handles branch statements */
void handleBranch(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMCallBr);

    if(LLVMGetNumOperands(instruction) == 1) {
        LLVMBasicBlockRef nextBlock = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 0);
        char* label = bbLabels[nextBlock];

        fprintf(fptr, "jmp %s", label);
    } else {

        LLVMValueRef cmp = LLVMGetOperand(instruction, 0);
        assert(LLVMGetNumOperands(pred) == 3);
        LLVMIntPredicate pred = LLVMGetOperand(cmp, 0);

        LLVMBasicBlockRef block1 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 1);
        char* label1 = bbLabels[block1];
        LLVMBasicBlockRef block2 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 2);
        char* label2 = bbLabels[block2];

        switch(pred) {
            case LLVMIntEQ:
                fprintf(fptr, "je %s\n", label1);
                break;
            case LLVMIntNE:
                fprintf(fptr, "jne %s\n", label1);
                break;
            case LLVMIntSGT:
                fprintf(fptr, "jg %s\n", label1);
                break;
            case LLVMIntSLT:
                fprintf(fptr, "jl %s\n", label1);
                break;
            case LLVMIntSGE:
                fprintf(fptr, "jge %s\n", label1);
                break;
            case LLVMIntSLE:
                fprintf(fptr, "jle %s\n", label1);
                break;
        }

        fprintf(fptr, "jmp %s\n", label2);
    }
}

/* handles alloca statements */
void handleAlloca(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMAlloca);
    return;
}

/* handles arithmetic statements */
void handleArithmetic(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMIsABinaryOperator(instruction));
    string reg;
    string op;

    if(regMap.count(instruction) != 0 && regMap[instruction] != "-1") {
        reg = regMap[instruction];
    } else {
        reg = "eax";
    }

    LLVMValueRef op1 = LLVMGetOperand(instruction, 1);
    LLVMValueRef op2 = LLVMGetOperand(instruction, 2);
    if(LLVMGetNumOperands(instruction == 2)) {
        op1 = LLVMGetOperand(instruction, 0);
        op2 = LLVMGetOperand(instruction, 1);

        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

        switch(opcode) {
            case LLVMAdd:
                op = "addl";
                break;
            case LLVMMul:
                op = "imull"
                break;
            case LLVMSub:
                op = "subl";
                break;
        }

    } else {
        op1 = LLVMGetOperand(instruction, 1);
        op2 = LLVMGetOperand(instruction, 2);

        op = "cmpl";
    }

    if(LLVMIsAConstantInt(op1)) {
        int val = LLVMConstIntGetSExtValue(op1);
        fprintf(fptr, "movl $%d, %s\n", val, reg.c_str());
    } else {
        assert(regMap.count(op1) != 0);

        if(regMap[op1] == "-1") {
            assert(offsetMap.count(op1) != 0);
            int offset = offsetMap[op1];

            fprintf(fptr, "movl %d(%%ebp), %s\n", offset, reg.c_str());

        } else {
            fprintf(fptr, "movl %s, %s\n", regMap[op1], reg);
        }
    }

        LLVMValueRef op2 = LLVMGetOperand(instruction, 2);

    if(LLVMIsAConstantInt(op2)) {
        int val = LLVMConstIntGetSExtValue(op2);
        fprintf(fptr, "%s $%d, %s\n", op, val, reg.c_str());
    } else {
        assert(regMap.count(op2) != 0);

        if(regMap[op2] == "-1") {
            assert(offsetMap.count(op2) != 0);
            int offset = offsetMap[op2];

            fprintf(fptr, "%s %d(%%ebp), %s\n", op, offset, reg.c_str());
        } else {
            fprintf(fptr, "%s %s, %s\n", op, regMap[op1], reg.c_str());
        }
    }

    if(reg == "eax") {
        assert(offsetMap.count(instruction) != 0);
        int offset = offsetMap[instruction];

        fprintf(fptr, "movl %%eax, %d(%%ebp)", offset);
    }

}