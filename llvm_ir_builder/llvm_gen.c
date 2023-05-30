/*
 * Library that contains methods to build LLVM IR from an ASTNode
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_gen.h"
#include "../helper/helper_functions.h"
#include <unordered_map>
#include <set>
#include <deque>
#include <string>
//#define NDEBUG
#include <cassert>

using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */

void traverseAST(astNode* node);
void getDeclarations(astNode* node);
void handleStatements_decs(astNode* node);

LLVMValueRef getLLVMCondition(astNode* node);
LLVMValueRef getLLVMExpression(astNode* node);
LLVMValueRef getTerm(astNode* node, bool negative);

void deadTerminatorElimination(LLVMValueRef function);
void deadBlockElimination(LLVMValueRef function);
void mergeLinearBlocks(LLVMValueRef function);
    void mergeBlocks(LLVMBasicBlockRef first, LLVMBasicBlockRef second);

void deleteBasicBlocks(vector<LLVMBasicBlockRef> blocks);


/* GLOBAL VARIABLES */
/* ---------------- */

unordered_map<string, LLVMValueRef> vars; 
LLVMValueRef func;
LLVMValueRef printFunc;
LLVMValueRef readFunc;
LLVMTypeRef printType;
LLVMTypeRef readType;
LLVMBasicBlockRef returnBlock;
LLVMBuilderRef builder;

unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>> bbOutGraph;
unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>> bbInGraph;


/* BUILD METHODS */
/* ------- */

/* main semantic analysis method - 
 * traverses nodes and handles them according to type 
 */
LLVMModuleRef createLLVMModelFromAST(astNode* root, char* filename) {  
    assert(root != NULL);

    // create module
    LLVMModuleRef mod = LLVMModuleCreateWithName("");
    LLVMSetTarget(mod, "x86_64-pc-linux-gnu");


    LLVMSetSourceFileName(mod, filename, strlen(filename));

    // create extern functions
    assert(root->prog.ext1 != NULL);
    if(root->prog.ext1 != NULL) {
        if(strcmp(root->prog.ext1->ext.name, "Print") == 0) {
            LLVMTypeRef param_types[] = { LLVMInt32Type() };
            printType = LLVMFunctionType(LLVMVoidType(), param_types, 1, 0);
            assert(root->prog.ext1->ext.name != NULL);
            printFunc = LLVMAddFunction(mod, root->prog.ext1->ext.name, printType);
        } else if(strcmp(root->prog.ext1->ext.name, "Read") == 0) {
            readType = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
            assert(root->prog.ext1->ext.name != NULL);
            readFunc = LLVMAddFunction(mod, root->prog.ext1->ext.name, readType);
        }
    }

    assert(root->prog.ext2 != NULL);
    if(root->prog.ext2 != NULL) {
        if(strcmp(root->prog.ext2->ext.name, "Print") == 0) {
            LLVMTypeRef param_types[] = { LLVMInt32Type() };
            printType = LLVMFunctionType(LLVMVoidType(), param_types, 1, 0);
            assert(root->prog.ext2->ext.name != NULL);
            printFunc = LLVMAddFunction(mod, root->prog.ext2->ext.name, printType);
        } else if(strcmp(root->prog.ext2->ext.name, "Read") == 0) {
            readType = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
            assert(root->prog.ext2->ext.name != NULL);
            readFunc = LLVMAddFunction(mod, root->prog.ext2->ext.name, readType);
        }
    }

    // create function with module
    assert(root->prog.func != NULL);
    if(root->prog.func->func.param != NULL) {
        LLVMTypeRef param_types[] = { LLVMInt32Type() };
        LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
        assert(root->prog.func->func.name);
        func = LLVMAddFunction(mod, root->prog.func->func.name, ret_type);
    } else {
        LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
        assert(root->prog.func->func.name);
        func = LLVMAddFunction(mod, root->prog.func->func.name, ret_type);
    }

    // build first basic block for function wrapper
    LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");
    builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);
    LLVMValueRef returnVal = LLVMBuildAlloca(builder, LLVMInt32Type(), "RETURN");
    LLVMSetAlignment(returnVal, 4);
    vars["return"] = returnVal;

    // initialize return block with return statement for return value
    returnBlock = LLVMAppendBasicBlock(func, "");
    LLVMPositionBuilderAtEnd(builder, returnBlock);
    LLVMValueRef toReturn = LLVMBuildLoad2(builder, LLVMInt32Type(), returnVal, "");
    LLVMBuildRet(builder, toReturn);
    LLVMPositionBuilderAtEnd(builder, first);

    // allocate
    if(root->prog.func->func.param != NULL) {
        assert(root->prog.func->func.param->var.name != NULL);
        LLVMValueRef param = LLVMBuildAlloca(builder, LLVMInt32Type(), root->prog.func->func.param->var.name);
        LLVMSetAlignment(param, 4);
        vars[root->prog.func->func.param->var.name] = param;
        assert(root->prog.func->func.body != NULL);
        getDeclarations(root->prog.func->func.body); // add all other allocations of variables in the program
        LLVMBuildStore(builder, LLVMGetParam(func, 0), param);
    } else {
        assert(root->prog.func->func.body != NULL);
        getDeclarations(root->prog.func->func.body); // add all other allocations of variables in the program
    }

    // traverse the ast
    traverseAST(root->prog.func->func.body);
    LLVMDisposeBuilder(builder);

    LLVMMoveBasicBlockAfter(returnBlock, LLVMGetLastBasicBlock(func));
   
    return mod;
}

/* traverses the statements of an ast and 
 * builds the basic blocks accordingly
 */
void traverseAST(astNode* node) {
    assert(node != NULL);

    assert(node->type == ast_stmt);
    switch(node->stmt.type) {

        case(ast_decl): {
            // declarations already handled before calling the method
            break;
        }

        case(ast_call): {
            assert(node->stmt.call.name != NULL);
            // if a lone PRINT instruction, build it (READ handled in expressions)
            if(strcmp(node->stmt.call.name, "Print") == 0) {
                assert(node->stmt.call.param != NULL);
                LLVMValueRef args[] = { getLLVMExpression(node->stmt.call.param) };
                LLVMBuildCall2(builder, printType, printFunc, args, 1, "");
            }
            break;
        }

        case(ast_ret): {
            // build a new basic block for the return
            LLVMBasicBlockRef assignRetVal_BB = LLVMAppendBasicBlock(func, "");
            LLVMBuildBr(builder, assignRetVal_BB); // branch to it

            // assign proper return value and branch to return block
            LLVMPositionBuilderAtEnd(builder, assignRetVal_BB);
            assert(node->stmt.ret.expr != NULL);
            LLVMValueRef expr = getLLVMExpression(node->stmt.ret.expr);
            LLVMBuildStore(builder, expr, vars["return"]);
            LLVMBuildBr(builder, returnBlock);

            break;
        }

        case(ast_while): {
            // create basic blocks
            LLVMBasicBlockRef condition_BB = LLVMAppendBasicBlock(func, "");
            LLVMBasicBlockRef body_BB = LLVMAppendBasicBlock(func, "");
            LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

            // build branch to condition
            LLVMBuildBr(builder, condition_BB);

            // create condition and looping structure
            LLVMPositionBuilderAtEnd(builder, condition_BB);
            assert(node->stmt.whilen.cond != NULL);
            LLVMValueRef condition = getLLVMCondition(node->stmt.whilen.cond);
            LLVMBuildCondBr(builder, condition, body_BB, final);

            LLVMPositionBuilderAtEnd(builder, body_BB);
            assert(node->stmt.whilen.body != NULL);
            traverseAST(node->stmt.whilen.body);
            LLVMBuildBr(builder, condition_BB);

            LLVMPositionBuilderAtEnd(builder, final);

            break;
        }

        case(ast_if): {
            // if body
            if(node->stmt.ifn.else_body != NULL) {
                // create basic blocks
                LLVMBasicBlockRef if_BB = LLVMAppendBasicBlock(func, "");
                LLVMBasicBlockRef else_BB = LLVMAppendBasicBlock(func, "");
                LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

                // create condition
                assert(node->stmt.ifn.cond != NULL);
                LLVMValueRef condition = getLLVMCondition(node->stmt.ifn.cond);
                LLVMBuildCondBr(builder, condition, if_BB, else_BB);

                // traverse through bodies of if, else, and final block
                LLVMPositionBuilderAtEnd(builder, if_BB);
                assert(node->stmt.ifn.if_body != NULL);
                traverseAST(node->stmt.ifn.if_body);
                LLVMBuildBr(builder, final);

                LLVMPositionBuilderAtEnd(builder, else_BB);
                traverseAST(node->stmt.ifn.else_body);
                LLVMBuildBr(builder, final);

                LLVMPositionBuilderAtEnd(builder, final);

            // if-else body
            } else {
                // create basic blocks
                LLVMBasicBlockRef if_BB = LLVMAppendBasicBlock(func, "");
                LLVMBasicBlockRef final = LLVMAppendBasicBlock(func, "");

                // create condition
                assert(node->stmt.ifn.cond != NULL);
                LLVMValueRef condition = getLLVMCondition(node->stmt.ifn.cond);
                LLVMBuildCondBr(builder, condition, if_BB, final);

                // traverse through bodies of if, and final block
                LLVMPositionBuilderAtEnd(builder, if_BB);
                assert(node->stmt.ifn.if_body != NULL);
                traverseAST(node->stmt.ifn.if_body);
                LLVMBuildBr(builder, final);

                LLVMPositionBuilderAtEnd(builder, final);
            }
            break;
        }
            
        case(ast_asgn): {
            // create an instruction for the given assignment statement
            assert(node->stmt.asgn.rhs != NULL);
            LLVMValueRef rhs = getLLVMExpression(node->stmt.asgn.rhs);
            assert(node->stmt.asgn.lhs != NULL);
            assert(node->stmt.asgn.lhs->var.name != NULL);
            LLVMBuildStore(builder, rhs, vars[string(node->stmt.asgn.lhs->var.name)]);
            break;
        }

        case(ast_block): {
            // loop through all statements and handle accordingly
            assert(node->stmt.block.stmt_list != NULL);
            vector<astNode*>* slist = node->stmt.block.stmt_list;
            vector<astNode*>::iterator it = slist->begin();
            while(it != slist->end()) {
                assert(*it != NULL);
                traverseAST(*it);
                it++;
            }
            break;
        }

    }

}

/* performs an initial traversal of the tree to get all declarations
 * so they can be allocated at the beginning of the function
 */
void getDeclarations(astNode* node) {
    assert(node != NULL);

    // loop through statements and handle them
    vector<astNode*>* slist = node->stmt.block.stmt_list;
    vector<astNode*>::iterator it = slist->begin();
    while(it != slist->end()) {
        assert(*it != NULL);
        astNode* node = (astNode*) *it;
        handleStatements_decs(node);        
        it++;
    }

}

/* a helper method that handles statements in the initial traversal, 
 * creating value refs for them when declarations are reached
 */
void handleStatements_decs(astNode* node) {
    assert(node != NULL);
    
    // traverse through all locations there could be declarations
    assert(node->type == ast_stmt);
    switch(node->stmt.type) {
            
            case(ast_block): {
                getDeclarations(node);
                break;
            }

            case(ast_if): {
                // handle if and else bodies
                assert(node->stmt.ifn.if_body != NULL);
                handleStatements_decs(node->stmt.ifn.if_body);
                if(node->stmt.ifn.else_body != NULL) {
                    handleStatements_decs(node->stmt.ifn.else_body);
                }
                break;
            }

            case(ast_while): {
                assert(node->stmt.whilen.body != NULL);
                handleStatements_decs(node->stmt.whilen.body);
                break;
            }

            case(ast_decl): {
                // if the variable has already been allocated, do not reallocate
                assert(node->stmt.decl.name != NULL);
                if(vars.count(node->stmt.decl.name)) {
                    break;
                }

                // allocate the variable and add it to the map of variables
                assert(node->stmt.decl.name != NULL);
                LLVMValueRef var = LLVMBuildAlloca(builder, LLVMInt32Type(), node->stmt.decl.name);
                LLVMSetAlignment(var, 4);
                vars[string(node->stmt.decl.name)] = var;
                break;
            }

            default:
                break;
        }
}

/* builds a condition and returns a value ref that corresponds
 * to said condition
 */
LLVMValueRef getLLVMCondition(astNode* node) {
    assert(node != NULL);
    
    LLVMValueRef cond;

    // build the left and right hand side
    assert(node->rexpr.lhs != NULL);
    LLVMValueRef lhs = getTerm(node->rexpr.lhs, false);
    assert(node->rexpr.rhs != NULL);
    LLVMValueRef rhs = getTerm(node->rexpr.rhs, false);

    // set the appropriate condition by operation
    switch(node->rexpr.op) {
        case(lt): {
            cond = LLVMBuildICmp(builder, LLVMIntSLT, lhs, rhs, "");
            break;
        }

        case(gt): {
            cond = LLVMBuildICmp(builder, LLVMIntSGT, lhs, rhs, "");
            break;
        }

        case(le): {
            cond = LLVMBuildICmp(builder, LLVMIntSLE, lhs, rhs, "");
            break;
        }

        case(ge): {
            cond = LLVMBuildICmp(builder, LLVMIntSGE, lhs, rhs, "");
            break;
        }

        case(eq): {
            cond = LLVMBuildICmp(builder, LLVMIntEQ, lhs, rhs, "");
            break;
        }

        case(neq): {
            cond = LLVMBuildICmp(builder, LLVMIntNE, lhs, rhs, "");
            break;
        }
    }

    return cond;

}

/* builds an expression and returns a value ref that corresponds
 * to said expression
 */
LLVMValueRef getLLVMExpression(astNode* node) {
    assert(node != NULL);

    LLVMValueRef expr;

    // set the expression depending on type 
    switch(node->type) {

        case(ast_var): {
            expr = getTerm(node, false);
            break;
        }

        case(ast_cnst): {
            expr = getTerm(node, false);
            break;
        }

        case(ast_bexpr): {
            
            // build the left and right hand side
            assert(node->bexpr.lhs != NULL);
            LLVMValueRef lhs = getTerm(node->bexpr.lhs, false);
            assert(node->bexpr.rhs != NULL);
            LLVMValueRef rhs = getTerm(node->bexpr.rhs, false);

            // build the arithmetic operator
            switch(node->bexpr.op) {
                case(add): {
                    expr = LLVMBuildAdd(builder, lhs, rhs, "");
                    break;
                }

                case(sub): {
                    expr = LLVMBuildSub(builder, lhs, rhs, "");
                    break;
                }

                case(divide): {
                    expr = LLVMBuildSDiv(builder, lhs, rhs, "");
                    break;
                }

                case(mul): {
                    expr = LLVMBuildMul(builder, lhs, rhs, "");
                    break;
                }

                default: {}
            }
            break;

        }

        case(ast_uexpr): {
            // if the uexpr is operating on a variable, multiply the variable by 
            assert(node->uexpr.expr != NULL);
            if(node->uexpr.expr->type == ast_var) {
                LLVMValueRef term = getTerm(node->uexpr.expr, false);
                LLVMValueRef neg1 = LLVMConstInt(LLVMInt32Type(), -1, false);
                expr = LLVMBuildMul(builder, term, neg1, "");
            } else {
                // otherwise just set the term as negative
                expr = getTerm(node->uexpr.expr, true);
            }
            break;
        }

        case(ast_stmt): { // if statement, must be an extern call
            assert(node->stmt.type == ast_call);
            assert(node->stmt.call.name != NULL);
            if(strcmp(node->stmt.call.name, "Read") == 0) {
                LLVMValueRef args[] = {};
                expr = LLVMBuildCall2(builder, readType, readFunc, args, 0, "");
            } else {
                expr = LLVMConstInt(LLVMInt32Type(), 1, false);
            }
            break;
        }

        default: {
            break;
        }

    }

    return expr;

}

/* gets a term as a value ref
 * will be either a constint or retrieving a variable using a load
 */
LLVMValueRef getTerm(astNode* node, bool negative) {
    assert(node != NULL);

    if(node->type == ast_var) { // variable - load
        assert(node->var.name != NULL);
        return LLVMBuildLoad2(builder, LLVMInt32Type(), vars[node->var.name], "");
    } else { // constant
        assert(node->type == ast_cnst);
        if(negative) {
            return LLVMConstInt(LLVMInt32Type(), -1 * node->cnst.value, false);
        } else {
            return LLVMConstInt(LLVMInt32Type(), node->cnst.value, false);
        }
    }
}


/* OPTIMIZATION FUNCTIONS */
/* ---------------------- */

/* optimizes the basic block from the generator */
void optimizeLLVMBasicBlocks(LLVMModuleRef mod) {
    
    // loop through the functions and optimize
    for(LLVMValueRef function =  LLVMGetFirstFunction(mod); 
        function; 
        function = LLVMGetNextFunction(function)) {

        deadTerminatorElimination(function);

        array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2> graphs = generateGraphs(function);
        bbOutGraph = graphs[0];
        bbInGraph = graphs[1];

        deadBlockElimination(function);
        mergeLinearBlocks(function);

    }
}

/* runs through all of the basic blocks
 * once a terminator is reached, it deletes all instructions after that terminator
*/
void deadTerminatorElimination(LLVMValueRef function) {
    assert(function != NULL);

    vector<LLVMValueRef> instructionsToErase; // instructions to erase

    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function);
        bb;
        bb = LLVMGetNextBasicBlock(bb)) {

        bool terminatorReached = false;
        // walk instructions
        for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); 
            instruction;
            instruction = LLVMGetNextInstruction(instruction)) {

            // if we have hit a branch or return, delete deadcode
            if(terminatorReached) {
                instructionsToErase.push_back(instruction);
            }

            // if we have a branch or return, enable flag to delete deadcode
            if(LLVMIsATerminatorInst(instruction)) {
                terminatorReached = true;
            }
        }

        // special case where no instructions in a block - add a return 0
        if(!LLVMGetFirstInstruction(bb)) {
            builder = LLVMCreateBuilder();
            LLVMPositionBuilderAtEnd(builder, bb);
            LLVMValueRef zero = LLVMConstInt(LLVMInt32Type(), 0, false);
            LLVMBuildRet(builder, zero);
            LLVMDisposeBuilder(builder);

        }
    }

    // delete the instructions
    vector<LLVMValueRef>::iterator it = instructionsToErase.begin();
    while(it != instructionsToErase.end()) {
        assert(*it != NULL);
        LLVMInstructionEraseFromParent(*it);
        it++;
    }
}

/* runs through the blocks and finds the set of 'reachable' blocks
 * deletes all blocks present in the function that are not 'reachable'
*/
void deadBlockElimination(LLVMValueRef function) {
    assert(function != NULL);

    vector<LLVMBasicBlockRef> blocksToDelete; // basic blocks to eventually delete
    set<LLVMBasicBlockRef> visitedBlocks; // all 'reachable' blocks
    deque<LLVMBasicBlockRef> queue; // queue for BFS

    // initialize queue
    if(!LLVMGetFirstBasicBlock(function)) return;
    queue.push_back(LLVMGetEntryBasicBlock(function));

    // bfs
    while(queue.size() != 0) {
        LLVMBasicBlockRef block = queue.back();
        queue.pop_back();
        visitedBlocks.insert(block);

        if(bbOutGraph.count(block) == 0) continue;

        // loop through successors
        set<LLVMBasicBlockRef> successors = bbOutGraph[block];
        set<LLVMBasicBlockRef>::iterator it = successors.begin();
        while(it != successors.end()) {
            LLVMBasicBlockRef successor = *it;
            if(visitedBlocks.count(successor) == 0) {
                queue.push_back(successor);
            }

            it++;
        }
    }

    // build list of unvisited blocks or blocks with no instructions
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function); 
        bb; 
        bb = LLVMGetNextBasicBlock(bb)) {
        
        if(visitedBlocks.count(bb) == 0) {
            blocksToDelete.push_back(bb);
        }

    }

    deleteBasicBlocks(blocksToDelete);
}

/* merges two blocks if they are both their unique successor and predecessor */
void mergeLinearBlocks(LLVMValueRef function) {
    assert(function != NULL);

    vector<LLVMBasicBlockRef> blocksToDelete;

    // loop through all of the basic blocks
    
    LLVMBasicBlockRef nextBlock;
    for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function);
        bb;
        bb = nextBlock) {
        
        nextBlock = bb;
        
        // skip bbs without successors
        if(bbOutGraph.count(bb) == 0) {
            nextBlock = LLVMGetNextBasicBlock(bb);
            continue;
        }

        set<LLVMBasicBlockRef> successors = bbOutGraph[bb];

        // skip bbs with more than one successor
        if(successors.size() != 1) {
            nextBlock = LLVMGetNextBasicBlock(bb);
            continue;
        }

        LLVMBasicBlockRef successor = *(successors.begin());
        assert(bbInGraph.count(successor) != 0);

        // skip bbs whose successor has more than one predecessor
        if(bbInGraph[successor].size() != 1) {
            nextBlock = LLVMGetNextBasicBlock(bb);
            continue;
        }

        // merge
        mergeBlocks(bb, successor);
        blocksToDelete.push_back(successor);

        // update graph
        bbOutGraph[bb].clear();
        
        set<LLVMBasicBlockRef> nextSuccessors = bbOutGraph[successor];
        set<LLVMBasicBlockRef>::iterator it = nextSuccessors.begin();
        while(it != nextSuccessors.end()) {
            bbOutGraph[bb].insert(*it);
            bbInGraph[*it].erase(successor);
            bbInGraph[*it].insert(bb);
            it++; 
        }

        bbOutGraph.erase(successor);
        bbInGraph.erase(successor);
    }

    deleteBasicBlocks(blocksToDelete);
}

/* merges two blocks together */
void mergeBlocks(LLVMBasicBlockRef first, LLVMBasicBlockRef second) {

    // delete all of the br instructions from the end of the first block
    vector<LLVMValueRef> instructionsToDelete;
    for(LLVMValueRef instruction = LLVMGetFirstInstruction(first);
        instruction;
        instruction = LLVMGetNextInstruction(instruction)) {

        if(LLVMGetInstructionOpcode(instruction) == LLVMBr) { // if br, add to list of instructions to delete
            instructionsToDelete.push_back(instruction);
        }
    }

    // delete
    vector<LLVMValueRef>::iterator it = instructionsToDelete.begin();
    while(it != instructionsToDelete.end()) {
        assert(*it != NULL);
        LLVMInstructionRemoveFromParent(*it);
        it++;
    }

    // add all of the instructions of the second block to the end of the first
    builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);
    LLVMValueRef nextInstruction;
    for(LLVMValueRef instruction = LLVMGetFirstInstruction(second);
        instruction;
        instruction = nextInstruction) {

        // remove from second and add to first
        nextInstruction = LLVMGetNextInstruction(instruction);
        LLVMInstructionRemoveFromParent(instruction);
        LLVMInsertIntoBuilder(builder, instruction);
    }

    LLVMDisposeBuilder(builder);
}

/* deletes a list of basic blocks from LLVM */
void deleteBasicBlocks(vector<LLVMBasicBlockRef> blocks) {
    // loop through and delete blocks
    vector<LLVMBasicBlockRef>::iterator it = blocks.begin();
    while(it != blocks.end()) {
        LLVMDeleteBasicBlock(*it);
        it++;
    }

}
