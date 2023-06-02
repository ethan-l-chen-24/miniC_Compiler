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
void printDirectives(LLVMValueRef func, char* filename);
void printFunctionEnd(LLVMValueRef func);
void getOffsetMap(LLVMValueRef func);
void printStack();

// instruction specific helper methods
void handleRet(LLVMValueRef instruction, LLVMValueRef func);
void handleLoad(LLVMValueRef instruction);
void handleStore(LLVMValueRef instruction, LLVMValueRef parameter);
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

unordered_map<LLVMBasicBlockRef, string> bbLabels;
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
        int index = -1;
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {
            index++;

            LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);
            
            // skip alloca instructions or any that do not return some value that are used
            // this covers store, branch, return instructions, as well as allocas and calls with void return types
            if(liveRange.count(instruction) == 0) {
                releaseOperands(instruction, index);
                continue;

            // if arithmetic, check for register transfer
            } else if(opcode == LLVMAdd || opcode == LLVMSub || opcode == LLVMMul) {
                assert(LLVMGetNumOperands(instruction) == 2);
                LLVMValueRef operand1 = LLVMGetOperand(instruction, 0);

                // check the second operand, and if it ends, return its register to register pool
                LLVMValueRef operand2 = LLVMGetOperand(instruction, 1);
                if(regMap.count(operand2) > 0 && regMap[operand2] != "-1") {
                    assert(liveRange.count(operand2) != 0);

                    // if this is that operands last use, we can transfer the register
                    if(liveRange[operand2][1] == index) {
                        regPool.push_back(regMap[operand2]);
                    }
                }

                // check if the operand is assigned to a register
                if(regMap.count(operand1) > 0 && regMap[operand1] != "-1") {
                    assert(liveRange.count(operand1) != 0);

                    // if this is that operands last use, we can transfer the register
                    if(liveRange[operand1][1] == index) {
                        regMap[instruction] = regMap[operand1];
                        continue;
                    }   
                } 
            } 
            
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
                } else {
                    regMap[instruction] = "-1";
                }

            }

            // release operands
            releaseOperands(instruction, index);
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
void codegen(LLVMModuleRef mod, char* filename) {
    assert(mod != NULL);
    assert(filename != NULL);

    fptr = fopen(filename, "w");

    // loop through each function
    for(LLVMValueRef func = LLVMGetFirstFunction(mod);
        func;
        func = LLVMGetNextFunction(func)) {

        if(!LLVMGetFirstBasicBlock(func)) {
            continue;
        }

        // get the parameter
        LLVMValueRef parameter;
        if(LLVMCountParams(func) == 1) {
            parameter = LLVMGetParam(func, 0);
        }

        // allocate registers and populate global variables
        createBBLabels(func);
        registerAllocation(func);
        printDirectives(func, filename);
        getOffsetMap(func);

        // loop through basic blocks
        for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
            bb;
            bb = LLVMGetNextBasicBlock(bb)) {

            fprintf(fptr, "%s:\n", bbLabels[bb].c_str());
            if(bb == LLVMGetFirstBasicBlock(func)) {
                printStack();
            }

            // loop through instructions
            for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
                instruction;
                instruction = LLVMGetNextInstruction(instruction)) {

                LLVMOpcode op = LLVMGetInstructionOpcode(instruction);

                // handle instruction types
                switch(op) {
                    case LLVMRet:
                        handleRet(instruction, func);
                        break;
                    case LLVMLoad:
                        handleLoad(instruction);
                        break;
                    case LLVMStore:
                        handleStore(instruction, parameter);
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
                    case LLVMICmp:
                        handleArithmetic(instruction);
                        break;
                    default:
                        break;
                }
            }

            fprintf(fptr, "\n");
        }

    }

    if(fptr != NULL) {
        fclose(fptr); 
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
            string firstLabel = ".LFB0";
            bbLabels[bb] = firstLabel;
        } else {
            char buf[36];
            sprintf(buf, ".L%d", i);

            // concatenate index to it
            bbLabels[bb] = string(buf);
        }

        i++;
    }
}

/* prints the directives of the function */
void printDirectives(LLVMValueRef func, char* filename) {
    assert(func != NULL);

    const char* functionName = LLVMGetValueName(func);

    // print directives
    fprintf(fptr, "\t.file\t\"%s\"\n", filename);
    fprintf(fptr, "\t.text\n");
    fprintf(fptr, "\t.globl\t%s\n", functionName);
    fprintf(fptr, "\t.type\t%s, @function\n", functionName);
    fprintf(fptr, "%s:\n", functionName);
}

/* prints the instructions to close a function */
void printFunctionEnd(LLVMValueRef func) {
    assert(func != NULL);

    fprintf(fptr, "\tpopl %%ebx\n");
    fprintf(fptr, "\tleave\n");
    fprintf(fptr, "\tret\n");
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

    // set local variable of parameter to have offset 8
    LLVMValueRef paramAlloca = NULL;
    if(LLVMCountParams(func) == 1) {
        // get the parameter
        LLVMValueRef param = LLVMGetParam(func, 0);
        
        // loop through for the store instruction 
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(LLVMGetFirstBasicBlock(func));
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {
            
            // if store instruction with store value of parameter
            if(LLVMGetInstructionOpcode(instruction) == LLVMStore) {
                if(LLVMGetOperand(instruction, 0) == param) {
                    // set the allocation address to be the second operand
                    paramAlloca = LLVMGetOperand(instruction, 1);
                    break;
                }
            }
        }

        offsetMap[paramAlloca] = 8;

    }

    // loop through all allocas instructions
    while(allocaInstruction != NULL && LLVMGetInstructionOpcode(allocaInstruction) == LLVMAlloca) {
        if(allocaInstruction == paramAlloca) {
            allocaInstruction = LLVMGetNextInstruction(allocaInstruction);
            continue;
        }
        offset += 4;
        offsetMap[allocaInstruction] = -offset;
        allocaInstruction = LLVMGetNextInstruction(allocaInstruction);
    }

     // loop through all instructions for loads to set offset to same as local var
    for(LLVMValueRef instruction = LLVMGetFirstInstruction(LLVMGetFirstBasicBlock(func));
        instruction;
        instruction = LLVMGetNextInstruction(instruction)) {
        
        // if store instruction with store value of parameter
        if(LLVMGetInstructionOpcode(instruction) == LLVMLoad) {
           assert(regMap.count(instruction) != 0);
           if(regMap[instruction] == "-1") {
                LLVMValueRef addr = LLVMGetOperand(instruction, 0);
                assert(offsetMap.count(addr) != 0);
                offsetMap[instruction] = offsetMap[addr];
           }
        }
    }

    // set global local mem value
    localMem = offset;
}

/* prints the offset map instructions at the opening of a function*/
void printStack() {
    // push base pointer
    fprintf(fptr, "\tpushl %%ebp\n");

    // move base pointer to stack pointer
    fprintf(fptr, "\tmovl %%esp, %%ebp\n");

    // move stack pointer to allocate enough space for allocas and spills
    fprintf(fptr, "\tsubl $%d, %%esp\n", localMem);

    // push ebx
    fprintf(fptr, "\tpushl %%ebx\n");
}

/* handles return statements */
void handleRet(LLVMValueRef instruction, LLVMValueRef func) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMRet);
    assert(LLVMGetNumOperands(instruction) == 1);

    // get return value
    LLVMValueRef retVal = LLVMGetOperand(instruction, 0);

    if(LLVMIsAConstantInt(retVal)) { // consntant
        int val = LLVMConstIntGetSExtValue(retVal);
        fprintf(fptr, "\tmovl $%d, %%eax\n", val);
    } else if(regMap.count(retVal) != 0 && regMap[retVal] == "-1") { // in memory
        int offset = offsetMap[retVal];
        fprintf(fptr, "\tmovl %d(%%ebp), %%eax\n", offset);
    } else if(regMap.count(retVal) != 0) { // in register
        fprintf(fptr, "\tmovl %%%s, %%eax\n", regMap[retVal].c_str());
    }

    printFunctionEnd(func); // TODO FILL IN
}

/* handles load statements */
void handleLoad(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMLoad);
    assert(LLVMGetNumOperands(instruction) == 1);

    // get load value
    LLVMValueRef loadVal = LLVMGetOperand(instruction, 0);

    if(regMap.count(instruction) != 0 && regMap[instruction] != "-1") { // load into register if from memory
        assert(offsetMap.count(loadVal) != 0);
        int offset = offsetMap[loadVal];
        fprintf(fptr, "\tmovl %d(%%ebp), %%%s\n", offset, regMap[instruction].c_str());
    } else {
        // load register to register
        assert(offsetMap.count(loadVal) != 0);
        assert(offsetMap.count(instruction) != 0);
        int offset1 = offsetMap[loadVal];
        int offset2 = offsetMap[instruction];
        fprintf(fptr, "\tmovl %d(%%ebp), %d(%%ebp)\n", offset1, offset2);
    }
}

/* handles store statements */
void handleStore(LLVMValueRef instruction, LLVMValueRef parameter) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMStore);
    assert(LLVMGetNumOperands(instruction) == 2);

    // get store value and destination
    LLVMValueRef storeVal = LLVMGetOperand(instruction, 0);
    LLVMValueRef destination = LLVMGetOperand(instruction, 1);
    assert(offsetMap.count(destination) != 0);

    // skip if it is a parameter
    if(storeVal == parameter) {
        return;
    }

    if(LLVMIsAConstantInt(storeVal)) { // constant store value
        int val = LLVMConstIntGetSExtValue(storeVal);
        int offset = offsetMap[destination];
        fprintf(fptr, "\tmovl $%d, %d(%%ebp)\n", val, offset);

    } else {

        if(regMap.count(storeVal) != 0 && regMap[storeVal] == "-1") { // store value in memory
            assert(offsetMap.count(storeVal) != 0);
            assert(offsetMap.count(destination) != 0);
            int valOffset = offsetMap[storeVal];
            int destinationOffset = offsetMap[destination];
            fprintf(fptr, "\tmovl %d(%%ebp), %%eax\n", valOffset);
            fprintf(fptr, "\tmovl %%eax, %d(%%ebp)\n", destinationOffset);
            
        } else { // store value in a register
            assert(offsetMap.count(destination) != 0);
            int offset = offsetMap[destination];
            fprintf(fptr, "\tmovl %%%s, %d(%%ebp)\n", regMap[storeVal].c_str(), offset);
        }
    }
}

/* handles call statements */
void handleCall(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMCall);

    // push registers to the stack
    fprintf(fptr, "\tpushl %%ebx\n");
    fprintf(fptr, "\tpushl %%ecx\n");
    fprintf(fptr, "\tpushl %%edx\n");

    LLVMValueRef calledFunc = LLVMGetCalledValue(instruction);
    int numParams = LLVMCountParams(calledFunc);

    const char* funcName = LLVMGetValueName(calledFunc);

    // if we have a parameter
    if(numParams == 1) {

        LLVMValueRef param = LLVMGetOperand(instruction, 0);

        // push the parameter to the stack
        if(LLVMIsAConstantInt(param)) { // constant parameter
            int val = LLVMConstIntGetSExtValue(param);
            fprintf(fptr, "\tpushl $%d\n", val);
        } else {
            assert(regMap.count(param) != 0);

            if(regMap[param] == "-1") { // parameter in memory
                assert(offsetMap.count(param) != 0);
                int offset = offsetMap[param];

                fprintf(fptr, "\tpushl %d(%%ebp)\n", offset);

            } else { // parameter in a register
                fprintf(fptr, "\tpushl %%%s\n", regMap[param].c_str());
            }
        }
    }

    // call the function
    fprintf(fptr, "\tcall %s\n", funcName);

    // if we have a parameter, remove that space from the stack
    if(numParams == 1) {
        fprintf(fptr, "\taddl $4, %%esp\n");
    }

    // pop off all of the registers
    fprintf(fptr, "\tpopl %%edx\n");
    fprintf(fptr, "\tpopl %%ecx\n");
    fprintf(fptr, "\tpopl %%ebx\n");

    // if returns a value
    if(regMap.count(instruction) != 0) {
        if(regMap[instruction] == "-1") {
            assert(offsetMap.count(instruction) != 0);
            int offset = offsetMap[instruction];
            fprintf(fptr, "\tmovl %%eax, %d(%%ebp)\n", offset);
        } else {
            fprintf(fptr, "\tmovl %%eax, %%%s\n", regMap[instruction].c_str());
        }
    }
}

/* handles branch statements */
void handleBranch(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMBr);

    // single branch
    if(LLVMGetNumOperands(instruction) == 1) {
        LLVMBasicBlockRef nextBlock = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 0);
        string label = bbLabels[nextBlock];

        fprintf(fptr, "\tjmp %s\n", label.c_str());

    } else { // conditional branch
        LLVMValueRef cmp = LLVMGetOperand(instruction, 0);

        // get both blocks
        LLVMBasicBlockRef block1 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 1);
        assert(bbLabels.count(block1) != 0);
        string label1 = bbLabels[block1];
        LLVMBasicBlockRef block2 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 2);
        assert(bbLabels.count(block2) != 0);
        string label2 = bbLabels[block2];

        // if const value predicate
        if(LLVMIsAConstantInt(cmp)) {
            if(LLVMConstIntGetSExtValue(cmp) == 0) {
                fprintf(fptr, "\tjmp %s\n", label1.c_str());
            } else {
                fprintf(fptr, "\tjmp %s\n", label2.c_str());
            }
            return;
        }

        assert(LLVMGetInstructionOpcode(cmp) == LLVMICmp);

        // get the predicate of the cmp instruction
        LLVMIntPredicate pred = LLVMGetICmpPredicate(cmp);

        // print jump depending on the predicate
        switch(pred) {
            case LLVMIntEQ:
                fprintf(fptr, "\tje %s\n", label1.c_str());
                break;
            case LLVMIntNE:
                fprintf(fptr, "\tjne %s\n", label1.c_str());
                break;
            case LLVMIntSGT:
                fprintf(fptr, "\tjl %s\n", label1.c_str());
                break;
            case LLVMIntSLT:
                fprintf(fptr, "\tjg %s\n", label1.c_str());
                break;
            case LLVMIntSGE:
                fprintf(fptr, "\tjle %s\n", label1.c_str());
                break;
            case LLVMIntSLE:
                fprintf(fptr, "\tjge %s\n", label1.c_str());
                break;
            default:
                break;
        }

        // print jump to other branch
        fprintf(fptr, "\tjmp %s\n", label2.c_str());
    }
}

/* handles alloca statements */
void handleAlloca(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMGetInstructionOpcode(instruction) == LLVMAlloca);

    // do nothing (handled by offsetMap)
    return;
}

/* handles arithmetic statements */
void handleArithmetic(LLVMValueRef instruction) {
    assert(instruction != NULL);
    assert(LLVMIsABinaryOperator(instruction) || LLVMGetInstructionOpcode(instruction) == LLVMICmp);

    string reg; // register to write to
    string op; // arithmetic operation

    // set reg if assigned a register
    if(regMap.count(instruction) != 0 && regMap[instruction] != "-1") {
        reg = regMap[instruction];
    } else {
        reg = "eax";
    }

    LLVMValueRef op1;
    LLVMValueRef op2;

    op1 = LLVMGetOperand(instruction, 0);
    op2 = LLVMGetOperand(instruction, 1);

    LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

    // get specific operand value
    switch(opcode) {
        case LLVMAdd:
            op = "addl";
            break;
        case LLVMMul:
            op = "imull";
            break;
        case LLVMSub:
            op = "subl";
            break;
        case LLVMICmp:
            op = "cmpl";
            break;
        default:
            break;
    }
    // if the first op is a constant
    if(LLVMIsAConstantInt(op1)) {
        int val = LLVMConstIntGetSExtValue(op1);
        fprintf(fptr, "\tmovl $%d, %%%s\n", val, reg.c_str());
    } else {
        assert(regMap.count(op1) != 0);

        if(regMap[op1] == "-1") { // in memory
            assert(offsetMap.count(op1) != 0);
            int offset = offsetMap[op1];

            fprintf(fptr, "\tmovl %d(%%ebp), %%%s\n", offset, reg.c_str());

        } else { // in a register
            if(regMap[op1] != regMap[instruction]) {
                fprintf(fptr, "\tmovl %%%s, %%%s\n", regMap[op1].c_str(), reg.c_str());
            }
        }
    }

    // if the second op is a constant
    if(LLVMIsAConstantInt(op2)) {
        int val = LLVMConstIntGetSExtValue(op2);
        fprintf(fptr, "\t%s $%d, %%%s\n", op.c_str(), val, reg.c_str());
    } else {
        assert(regMap.count(op2) != 0);

        if(regMap[op2] == "-1") { // in memory
            assert(offsetMap.count(op2) != 0);
            int offset = offsetMap[op2];

            fprintf(fptr, "\t%s %d(%%ebp), %%%s\n", op.c_str(), offset, reg.c_str());

        } else { // in a register
            fprintf(fptr, "\t%s %%%s, %%%s\n", op.c_str(), regMap[op2].c_str(), reg.c_str());
        }
    }

    if(reg == "eax") { // if assigned a temp register, put in memory
        assert(offsetMap.count(instruction) != 0);
        int offset = offsetMap[instruction];

        fprintf(fptr, "\tmovl %%eax, %d(%%ebp)\n", offset);
    }

}