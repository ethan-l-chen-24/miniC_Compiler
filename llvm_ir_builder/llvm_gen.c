/*
 * Library that contains methods to build LLVM IR from an ASTNode
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_gen.h"
#include <map>
#include<string>

using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */

void traverseAST(astNode* node);
void getDeclarations(astNode* node);
void handleStatements_decs(astNode* node);

LLVMValueRef getLLVMCondition(astNode* node);
LLVMValueRef getLLVMExpression(astNode* node);
LLVMValueRef getTerm(astNode* node, bool negative);


/* GLOBAL VARIABLES */
/* ---------------- */

map<string, LLVMValueRef> vars; 
LLVMValueRef func;
LLVMValueRef printFunc;
LLVMValueRef readFunc;
LLVMTypeRef printType;
LLVMTypeRef readType;
LLVMBasicBlockRef returnBlock;
LLVMBuilderRef builder;

/* METHODS */
/* ------- */

/* main semantic analysis method - 
 * traverses nodes and handles them according to type 
 */
LLVMModuleRef createLLVMModelFromAST(astNode* root) {  
    // create module
    LLVMModuleRef mod = LLVMModuleCreateWithName("");
    LLVMSetTarget(mod, "x86_64-pc-linux-gnu");

    // create extern functions
    if(root->prog.ext1 != NULL) {
        if(strcmp(root->prog.ext1->ext.name, "Print") == 0) {
            LLVMTypeRef param_types[] = { LLVMInt32Type() };
            printType = LLVMFunctionType(LLVMVoidType(), param_types, 1, 0);
            printFunc = LLVMAddFunction(mod, root->prog.ext1->ext.name, printType);
        } else if(strcmp(root->prog.ext1->ext.name, "Read") == 0) {
            readType = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
            readFunc = LLVMAddFunction(mod, root->prog.ext1->ext.name, readType);
        }
    }

    if(root->prog.ext2 != NULL) {
        if(strcmp(root->prog.ext2->ext.name, "Print") == 0) {
            LLVMTypeRef param_types[] = { LLVMInt32Type() };
            printType = LLVMFunctionType(LLVMVoidType(), param_types, 1, 0);
            printFunc = LLVMAddFunction(mod, root->prog.ext2->ext.name, printType);
        } else if(strcmp(root->prog.ext2->ext.name, "Read") == 0) {
            readType = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
            readFunc = LLVMAddFunction(mod, root->prog.ext2->ext.name, readType);
        }
    }

    // create function with module
    if(root->prog.func->func.param != NULL) {
        LLVMTypeRef param_types[] = { LLVMInt32Type() };
        LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
        func = LLVMAddFunction(mod, root->prog.func->func.name, ret_type);
    } else {
        LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), {}, 0, 0);
        func = LLVMAddFunction(mod, root->prog.func->func.name, ret_type);
    }

    // build first basic block for function wrapper
    LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");
    builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, first);
    LLVMValueRef returnVal = LLVMBuildAlloca(builder, LLVMInt32Type(), "RETURN");
    vars["return"] = returnVal;

    // initialize return block with return statement for return value
    returnBlock = LLVMAppendBasicBlock(func, "");
    LLVMPositionBuilderAtEnd(builder, returnBlock);
    LLVMBuildRet(builder, returnVal);
    LLVMPositionBuilderAtEnd(builder, first);

    // allocate
    if(root->prog.func->func.param != NULL) {
        LLVMValueRef param = LLVMBuildAlloca(builder, LLVMInt32Type(), root->prog.func->func.param->var.name);
        LLVMSetAlignment(param, 4);
        vars[root->prog.func->func.param->var.name] = param;
        getDeclarations(root->prog.func->func.body); // add all other allocations of variables in the program
        LLVMBuildStore(builder, LLVMGetParam(func, 0), param);
    } else {
        getDeclarations(root->prog.func->func.body); // add all other allocations of variables in the program
    }

    // traverse the ast
    traverseAST(root->prog.func->func.body);
    LLVMDisposeBuilder(builder);
   
    return mod;
}

/* traverses the statements of an ast and 
 * builds the basic blocks accordingly
 */
void traverseAST(astNode* node) {

    switch(node->stmt.type) {

        case(ast_decl): {
            // declarations already handled before calling the method
            break;
        }

        case(ast_call): {
            if(strcmp(node->stmt.call.name, "Print") == 0) {
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
            LLVMValueRef condition = getLLVMCondition(node->stmt.whilen.cond);
            LLVMBuildCondBr(builder, condition, body_BB, final);

            LLVMPositionBuilderAtEnd(builder, body_BB);
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
                LLVMValueRef condition = getLLVMCondition(node->stmt.ifn.cond);
                LLVMBuildCondBr(builder, condition, if_BB, else_BB);

                // traverse through bodies of if, else, and final block
                LLVMPositionBuilderAtEnd(builder, if_BB);
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
                LLVMValueRef condition = getLLVMCondition(node->stmt.ifn.cond);
                LLVMBuildCondBr(builder, condition, if_BB, final);

                // traverse through bodies of if, and final block
                LLVMPositionBuilderAtEnd(builder, if_BB);
                traverseAST(node->stmt.ifn.if_body);
                LLVMBuildBr(builder, final);

                LLVMPositionBuilderAtEnd(builder, final);
            }
            break;
        }
            
        case(ast_asgn): {
            // create an instruction for the given assignment statement
            LLVMValueRef rhs = getLLVMExpression(node->stmt.asgn.rhs);
            LLVMBuildStore(builder, rhs, vars[string(node->stmt.asgn.lhs->var.name)]);
            break;
        }

        case(ast_block): {
            // loop through all statements and handle accordingly
            vector<astNode*>* slist = node->stmt.block.stmt_list;
            vector<astNode*>::iterator it = slist->begin();
            while(it != slist->end()) {
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

    // loop through statements and handle them
    vector<astNode*>* slist = node->stmt.block.stmt_list;
    vector<astNode*>::iterator it = slist->begin();
    while(it != slist->end()) {
        astNode* node = (astNode*) *it;
        handleStatements_decs(node);        
        it++;
    }

}

/* a helper method that handles statements in the initial traversal, 
 * creating value refs for them when declarations are reached
 */
void handleStatements_decs(astNode* node) {
    
    // traverse through all locations there could be declarations
    switch(node->stmt.type) {
            
            case(ast_block): {
                getDeclarations(node);
                break;
            }

            case(ast_if): {
                // handle if and else bodies
                handleStatements_decs(node->stmt.ifn.if_body);
                if(node->stmt.ifn.else_body != NULL) {
                    handleStatements_decs(node->stmt.ifn.else_body);
                }
                break;
            }

            case(ast_while): {
                handleStatements_decs(node->stmt.whilen.body);
                break;
            }

            case(ast_decl): {
                // if the variable has already been allocated, do not reallocate
                if(vars.count(node->stmt.decl.name) == 1) {
                    break;
                }

                // allocate the variable and add it to the map of variables
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
    
    LLVMValueRef cond;

    // build the left and right hand side
    LLVMValueRef lhs = getTerm(node->rexpr.lhs, false);
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
            LLVMValueRef lhs = getTerm(node->bexpr.lhs, false);
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

        case(ast_stmt): { // TODO FIX THIS DEFAULT FOR EXTERN CALL
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
    if(node->type == ast_var) { // variable - load
        return LLVMBuildLoad2(builder, LLVMInt32Type(), vars[node->var.name], "");
    } else { // constant
        if(negative) {
            return LLVMConstInt(LLVMInt32Type(), -1 * node->cnst.value, false);
        } else {
            return LLVMConstInt(LLVMInt32Type(), node->cnst.value, false);
        }
    }
}
