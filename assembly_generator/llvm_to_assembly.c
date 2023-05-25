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

void getIndices(LLVMBasicBlockRef bb);
void computeLiveness(LLVMBasicBlockRef bb);
void sortList();
void registerAllocation(LLVMValueRef func);
    LLVMValueRef findSpill();
    void releaseOperands(LLVMValueRef instruction, int index);

void createBBLabels();
void printDirectives();
void printFunctionEnd();
void getOffsetMap();


/* GLOBAL VARIABLES */
/* ---------------- */
unordered_map<LLVMValueRef, int> instIndex;
unordered_map<LLVMValueRef, array<int, 2>> liveRange;
vector<LLVMValueRef> sortedList;
unordered_map<LLVMValueRef, string> regMap;
vector<string> regPool;


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

/*
*/
void codegen(LLVMModuleRef mod, char* filepath) {
    assert(mod != NULL);

    // loop through each function
    for(LLVMValueRef func = LLVMGetFirstFunction(mod);
        func;
        func = LLVMGetNextFunction(func)) {

        registerAllocation(func);

        for(auto i : regMap) {
            LLVMDumpValue(i.first);
            printf("\n");
            cout << i.second;
            printf("\n");
        }

        bool here = true;

    }

}
