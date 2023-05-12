/*
 * Library that contains methods to optimize a LLVM IR module
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_optimizations.h"
#include "../helper/helper_functions.h"
#include <unordered_map>
#include <vector>
#include <set>
#define NDEBUG
#include <cassert>

using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */
void getAllFunctionGraphs(LLVMModuleRef mod);
void getAllOperands(LLVMModuleRef mod);
bool runLocalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMBasicBlockRef bb));
bool runGlobalOptimizations(LLVMModuleRef mod, bool (*opt)(LLVMValueRef func));
bool eraseInstructions(vector<LLVMValueRef>* instructions);

void generateStoreSet(LLVMValueRef func);
void generateGen(LLVMValueRef func);
void generateKill(LLVMValueRef func);
void computeInOut(LLVMValueRef func);
    void generateIn(LLVMBasicBlockRef bb);
    void generateOut(LLVMBasicBlockRef bb);
bool performConstantProp(LLVMValueRef func);

/* GLOBAL VARIABLES */
/* ---------------- */
set<LLVMValueRef> allOperands;
unordered_map<LLVMBasicBlockRef, set<LLVMValueRef>> gen;
unordered_map<LLVMBasicBlockRef, set<LLVMValueRef>> kill;
unordered_map<LLVMBasicBlockRef, set<LLVMValueRef>> in;
unordered_map<LLVMBasicBlockRef, set<LLVMValueRef>> out;
unordered_map<LLVMValueRef, set<LLVMValueRef>> storeSet;

unordered_map<LLVMValueRef, array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2>> graphs;

/* FUNCTIONS */
/* --------- */

/* runs all of the necessary optimizations on the LLVMModule such that
 * as many unnecessary instructions as possible are eliminated
 */
void optimizeLLVM(LLVMModuleRef mod) {
    assert(mod != NULL);

    getAllFunctionGraphs(mod);

    // run constant folding and constant propagation until no more changes
    bool changed = true;
    while(changed) {
        changed = false;
        allOperands.clear();
        getAllOperands(mod);
        changed |= runLocalOptimizations(mod, deadCodeElimination);
        changed |= runLocalOptimizations(mod, commonSubexpressionElimination);
        changed |= runLocalOptimizations(mod, constantFolding);
        changed |= runGlobalOptimizations(mod, constantPropagation);
    }

}

/* generate all of the graphs for each function and put it in a map */
void getAllFunctionGraphs(LLVMModuleRef mod) {
    // loop through all the functions
    for(LLVMValueRef function = LLVMGetFirstFunction(mod);
        function;
        function = LLVMGetNextFunction(function)) {
            // generate the graphs
            graphs[function] = generateGraphs(function);
    }
}

/* adds all instructions to the global set*/
void getAllOperands(LLVMModuleRef mod) {
    assert(mod != NULL);

    // loop through all functions
     for(LLVMValueRef function = LLVMGetFirstFunction(mod); 
        function; 
        function = LLVMGetNextFunction(function)) {

        // loop through all basic blocks
        for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
            basicBlock;
            basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
                
            // loop through all instructions
            for(LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
                instruction;
                instruction = LLVMGetNextInstruction(instruction)) {

                // loop through operands and add to set
                for(int i = 0; i < LLVMGetNumOperands(instruction); i++) {
                    allOperands.insert(LLVMGetOperand(instruction, i));
                }
            }
        }
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

        instructions->clear();

        return true;
    }
}

/* LOCAL OPTIMIZATION FUNCTIONS */
/* ---------------------------- */

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
        if(opcode == LLVMAlloca || opcode == LLVMCall) {
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

        // otherwise, loop through the instructions and look for common operands
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

    // delete all marked instructions
    bool changed = eraseInstructions(instructionsToErase);
    delete(instructionsToErase);

    return changed;
}

/* Finds instructions that have no use
 * these instructions will follow a return or branch
 * or never be used in the instruction list
 */
bool deadCodeElimination(LLVMBasicBlockRef bb) { // TODO MAKE THIS BETTER
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
        } else {

            if(opcode == LLVMCall || opcode == LLVMAlloca || opcode == LLVMStore) {
                continue;
            }

            // if any other instruction, must be 
            if(allOperands.count(instruction) == 0) {
                instructionsToErase->push_back(instruction);
            }
        }
    }

    // delete all marked instructions
    bool changed = eraseInstructions(instructionsToErase);

    // continue to delete deadcode created by deleting deadcode until no more
    if(changed) {
        for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            // retrieve the opCode
            LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

            if(opcode == LLVMRet || LLVMBr || opcode == LLVMCall || opcode == LLVMAlloca || opcode == LLVMStore) {
                continue;
            }

            if(allOperands.count(instruction) == 0) {
                instructionsToErase->push_back(instruction);
            }
        }
        changed = eraseInstructions(instructionsToErase);
    }

    delete(instructionsToErase);

    return changed;
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

    // delete all marked instructions
    bool changed = eraseInstructions(instructionsToErase);
    delete(instructionsToErase);

    return changed;
}

/* GLOBAL OPTIMIZATION INSTRUCTIONS */
/* -------------------------------- */

/* performs constant propagation, an optimization technique that 
 * replaces all variables with the constant its value is defined as, assuming
 * it is constant across all of its reaches
 */
bool constantPropagation(LLVMValueRef func) {
    generateStoreSet(func);
    generateGen(func);
    generateKill(func);
    computeInOut(func);
    return performConstantProp(func);
}

/* generates the 'storeSet' map, which maps addresses to 
 * its store instructions across all basic blocks
 */
void generateStoreSet(LLVMValueRef func) {
    storeSet.clear();

    // loop through all basic blocks
    for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
        basicBlock;
        basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

        // loop through all instructions
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            // if we have a store instruction
            if(LLVMGetInstructionOpcode(instruction) == LLVMStore) {
                LLVMValueRef addr = LLVMGetOperand(instruction, 1);

                // store the instruction in its address bucket
                if(storeSet.count(addr) == 0) { // create bucket if doesn't exist
                    set<LLVMValueRef> storeAddr;
                    storeSet[addr] = storeAddr;
                }
                storeSet[addr].insert(instruction);
            }

        }
    }
}

/* generates the 'gen' map, which maps each basic block
 * to its set of 'gen' instructions, or store instructions
 * that are not overwritten and escape the basic block
 */
void generateGen(LLVMValueRef func) {
    gen.clear();

    // loop through basic blocks
    for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
        basicBlock;
        basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

        // create bucket for basic block
        set<LLVMValueRef> genBucket;
        gen[basicBlock] = genBucket;

        // keeps track of the store instructions to each addr iterated over in the
        // current basic block
        unordered_map<LLVMValueRef, LLVMValueRef> activeStores;

        // loop over instructions
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {
            
            // if we have a store instruction
            if(LLVMGetInstructionOpcode(instruction) == LLVMStore) {
                LLVMValueRef addr = LLVMGetOperand(instruction, 1);

                // if we have already seen this instruction address in this block, erase it from gen
                if(activeStores.count(addr) != 0) {
                    assert(gen[basicBlock].count(activeStores[addr]) != 0);
                    gen[basicBlock].erase(activeStores[addr]);
                    activeStores[addr] = instruction;
                }

                // add current instruction to gen
                gen[basicBlock].insert(instruction);
            }
        }
    }
}

/* generates the 'kill' map, which maps each basic block
 * to its set of 'kill' instructions, or store instructions
 * killed by its own stores
 */
void generateKill(LLVMValueRef func) {
    kill.clear();

    // loop through all basic blocks
    for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
        basicBlock;
        basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

        // create bucket for basic block
        set<LLVMValueRef> killBucket;
        gen[basicBlock] = killBucket;

        // loop through instructions
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            // if we have a store instruction
            if(LLVMGetInstructionOpcode(instruction) == LLVMStore) {
                LLVMValueRef addr = LLVMGetOperand(instruction, 1);
                assert(storeSet.count(addr) != 0);
                
                // iterate through all instructions with same address and add to kill set
                set<LLVMValueRef> addrSet = storeSet[addr];
                set<LLVMValueRef>::iterator it = addrSet.begin();
                while(it != addrSet.end()) {
                    if(*it != instruction) { // ensure not the same instruction
                        gen[basicBlock].insert(*it);
                    }
                    it++;
                }
            }
        }
    }
}

/* iteratively compute the 'in' and 'out' blocks for each basic block */
void computeInOut(LLVMValueRef func) {
    in.clear();
    out.clear();

    // set out to in
    for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
        basicBlock;
        basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

        out[basicBlock] = gen[basicBlock];
    }

    // iterate in and out as long as there are changes
    bool changed = true;
    while(changed) {
        changed = false;

        // loop through all the basic blocks
        for(LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(func);
            basicBlock;
            basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

            // generate in and out
            generateIn(basicBlock);
            set<LLVMValueRef> oldout = out[basicBlock];
            generateOut(basicBlock);

            // if no changes, we are done
            if(out[basicBlock] != oldout) {
                changed = true;
            }
        }
    }
}

/* generate 'in' for the particular basic block -
 * in[bb] = union(out[p1], out[p2], ..., out[pn]) where pi is a predecessor
 */
void generateIn(LLVMBasicBlockRef bb) {
    in[bb].clear();

    // get the parent function, graph, and set of predecessors
    LLVMValueRef function = LLVMGetBasicBlockParent(bb);
    unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>> inGraph = graphs[function][1];
    assert(inGraph.count(bb) != 0);
    set<LLVMBasicBlockRef> predecessors = inGraph[bb];

    // loop through the predecessors and all of their outs
    set<LLVMBasicBlockRef>::iterator it = predecessors.begin();
    while(it != predecessors.end()) {
        assert(out.count(bb) != 0);

        // loop through all of the outs of the predecessors
        set<LLVMValueRef>::iterator outIt = out[bb].begin();
        while(outIt != out[bb].end()) {
            // insert instructions into in
            in[bb].insert(*outIt);
            outIt++;
        }

        it++;
    }
}

/* generate 'out' for the particular basic block -
 * out[bb] = gen[bb] U (in[bb] - kill[bb])
 */
void generateOut(LLVMBasicBlockRef bb) {
    out[bb].clear();

    // copy in
    set<LLVMValueRef> inDiffKill = in[bb];

    // calculate in[bb] - kill[bb]
    set<LLVMValueRef>::iterator killIt = kill[bb].begin();
    while(killIt != kill[bb].end()) {
        // if in[bb] contains the item from kill, remove it
        if(inDiffKill.count(*killIt) != 0) {
            inDiffKill.erase(*killIt);
        }

        killIt++;
    }

    // add all of gen[bb] to out[bb]
    set<LLVMValueRef>::iterator genIt = gen[bb].begin();
    while(genIt != gen[bb].end()) {
        out[bb].insert(*genIt);
        genIt++;
    }
    
    // add all of inDiffKill to out[bb]
    set<LLVMValueRef>::iterator inDiffKillIt = inDiffKill.begin();
    while(inDiffKillIt != inDiffKill.end()) {
        out[bb].insert(*inDiffKillIt);
        inDiffKillIt++;
    }
}

/* edit the instruction set according to the in, out, kill, and gen sets */
bool performConstantProp(LLVMValueRef func) {
    
    vector<LLVMValueRef>* instructionsToErase = new vector<LLVMValueRef>(); // instructions to erase
    
    // loop over all of the basic blocks
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
        bb;
        bb = LLVMGetNextBasicBlock(bb)) {

        // set r as in[bb]
        set<LLVMValueRef> r = in[bb];

        // loop over all instructions
        for(LLVMValueRef instruction = LLVMGetFirstInstruction(bb);
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            LLVMOpcode op = LLVMGetInstructionOpcode(instruction);

            // case 1: store
            if(op == LLVMStore) { 

                r.insert(instruction);
                LLVMValueRef addr = LLVMGetOperand(instruction, 1); // store address

                // loop over all instructions in r
                set<LLVMValueRef>::iterator rIt = r.begin();
                while(rIt != r.end()) {
                    assert(LLVMGetInstructionOpcode(*rIt) == LLVMStore);
                    if(*rIt == instruction) {
                        continue;
                    }

                    // if it has the same store address, killed by instruction, remove from r
                    if(addr == LLVMGetOperand(*rIt, 1)) {
                        r.erase(*rIt);
                    }

                    rIt++;
                }
                
            // case 2: load
            } else if(op == LLVMLoad) { 
                LLVMValueRef addr = LLVMGetOperand(instruction, 0); // load address

                // loop over all instructions in r and keep track of constant int values
                bool propagate = true; // if true at end of while loop, we can propagate the constant int value
                bool firstConstFound = false; // checks for first constant to be set as constValue
                LLVMValueRef constValue; // store the constant value

                set<LLVMValueRef>::iterator rIt = r.begin();
                while(rIt != r.end()) {
                    assert(LLVMGetInstructionOpcode(*rIt) == LLVMStore);

                    // ignore stores to other addresses
                    if(addr != LLVMGetOperand(*rIt, 1)) {
                        continue;
                    }

                    // get the value that is stored
                    LLVMValueRef storedValue = LLVMGetOperand(*rIt, 0);

                    // if it isn't a constant, we cannot propagate
                    if(!LLVMIsAConstantInt(storedValue)) {
                        propagate = false;
                        break;
                    }

                    // assign first constant value
                    if(!firstConstFound) {
                        constValue = storedValue;
                    }

                    // if two different constant values are stored, we cannot propagate
                    if(LLVMConstIntGetSExtValue(storedValue) != LLVMConstIntGetSExtValue(constValue)) {
                        propagate = false;
                        break;
                    }

                    rIt++;
                }

                // if we have a constant value to propagate
                if(propagate && firstConstFound) {
                    LLVMReplaceAllUsesWith(instruction, constValue);
                    instructionsToErase->push_back(instruction);
                }
            }
        }
    }

    // delete marked load instructions
    bool changed = eraseInstructions(instructionsToErase);
    delete(instructionsToErase);

    return changed;
}
