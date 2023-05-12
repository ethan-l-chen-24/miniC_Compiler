#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helper_functions.h"

using namespace std;

/* GRAPH BUILDER */
/* ------------- */

/* generates an in and out-graph for the Basic Blocks.
 * returns them in an array where the first index corresponds to the out-graph
 * and the second index refers to the in-graph 
 */
array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2> generateGraphs(LLVMValueRef func) {

    // create graphs
    unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>> outGraph;
    unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>> inGraph;

    // loop through basic blocks
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func);
        bb;
        bb = LLVMGetNextBasicBlock(bb)) {

        // get the terminator to see branch instructions
        LLVMValueRef instruction = LLVMGetBasicBlockTerminator(bb);
        if(!instruction) {
            continue;
        }

        // create entry in out graph
        set<LLVMBasicBlockRef> successors;
        outGraph[bb] = successors;

        LLVMOpcode opcode = LLVMGetInstructionOpcode(instruction);

        switch(opcode) {
            case(LLVMBr): {
                if(LLVMGetNumOperands(instruction) > 1) { // conditional branch

                    LLVMBasicBlockRef dest1 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 1);
                    LLVMBasicBlockRef dest2 = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 2);

                    // insert in out graph
                    outGraph[bb].insert(dest1);
                    outGraph[bb].insert(dest2);

                    // if in graph doesn't have the basic blocks, add them
                    if(inGraph.count(dest1) == 0) {
                        set<LLVMBasicBlockRef> predecessors;
                        inGraph[dest1] = predecessors;
                    }

                    if(inGraph.count(dest2) == 0) {
                        set<LLVMBasicBlockRef> predecessors;
                        inGraph[dest2] = predecessors;
                    }

                    // insert in in graph
                    inGraph[dest1].insert(bb);
                    inGraph[dest2].insert(bb);


                } else { // direct branch
                    LLVMBasicBlockRef dest = (LLVMBasicBlockRef) LLVMGetOperand(instruction, 0);

                    // insert in out graph
                    outGraph[bb].insert(dest); 

                    // if in graph doesn't have the basic block, add it
                    if(inGraph.count(dest) == 0) {
                        set<LLVMBasicBlockRef> predecessors;
                        inGraph[dest] = predecessors;
                    }

                    // insert in in graph
                    inGraph[dest].insert(bb);
                }
            }

            default: {
                break;
            }
        }
        
    }

    // return graphs
    array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2> graphs;
    graphs[0] = outGraph;
    graphs[1] = inGraph;
    return graphs;

}
