/* 
 * Library of semantic analysis functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define NDEBUG
#include <cassert>
#include "semantic_analysis.h"
using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */
// non-optimized
bool handleStatements(astNode* node);
bool onSymbolTable(char* var);

// optimized
bool handleStatements_opt(astNode* node);
bool onSymbolTable_opt(string var);
bool onFrontSymbolTable_opt(char* var);
void deleteNonActiveSymbols_opt(vector<char*>* symbols);

/* GLOBAL VARS */
/* ----------- */

deque<vector<char*>*> stack; // stores symbol tables
set<string> activeSymbols; // for optimization

/* METHODS */
/* ------- */

/* main semantic analysis method - 
 * traverses nodes and handles them according to type 
*/
bool semanticAnalysis(astNode* node) {  
    assert(node != NULL);

    bool result = true;
    // react based on node types
    switch(node->type) {

        // traverse nodes
        case(ast_prog):
            assert(node->prog.ext1 != NULL);
            result &= semanticAnalysis(node->prog.ext1); // unnecessary
            assert(node->prog.ext2 != NULL);
            result &= semanticAnalysis(node->prog.ext2); // unnecessary
            assert(node->prog.func != NULL);
            result &= semanticAnalysis(node->prog.func);
            break;

        // handle block and declaration statements
        case(ast_stmt):
            result &= handleStatements(node);
            break;

        // do nothing
        case(ast_extern):
            break;

        // check if the variable is on the symbol table, if not print an error
        case(ast_var):
            assert(node->var.name != NULL);
            if(!onSymbolTable(node->var.name)) {
                fprintf(stdout, "Symbol error: var [%s] has not been declared\n\n", node->var.name);
                result = false;
            }
            break;

        // do nothing
        case(ast_cnst):
            break;

        // traverse nodes
        case(ast_rexpr):
            assert(node->rexpr.lhs != NULL);
            result &= semanticAnalysis(node->rexpr.lhs);
            assert(node->rexpr.rhs != NULL);
            result &= semanticAnalysis(node->rexpr.rhs);
            break;

        // traverse nodes
        case(ast_bexpr):
            assert(node->bexpr.lhs != NULL);
            result &= semanticAnalysis(node->bexpr.lhs);
            assert(node->bexpr.rhs != NULL);
            result &= semanticAnalysis(node->bexpr.rhs);
            break;

        // traverse nodes
        case(ast_uexpr):
            assert(node->uexpr.expr != NULL);
            result &= semanticAnalysis(node->uexpr.expr);
            break;

        // handle parameters
        case(ast_func):
            // create new var list and add parameter if it exists
            vector<char*>* current_symbols = new vector<char*>();
            if(node->func.param != NULL) {
                stack.push_front(current_symbols);
                assert(node->func.param->var.name != NULL);
                current_symbols->push_back(node->func.param->var.name);
            }

            assert(node->func.body != NULL);
            result &= semanticAnalysis(node->func.body); // traverse into body of function

            // free the front symbol vector
            stack.pop_front();
            delete(current_symbols);
            break;

    }
    
    return result;
}

/* handles all of the possible statement types */
bool handleStatements(astNode* node) {
    assert(node != NULL);
   
    bool result = true;
    // react based on statement types
    switch(node->stmt.type) {

        // if a declaration, add var to list in top of stack
        case(ast_decl):
            assert(node->stmt.decl.name != NULL);
            stack.front()->push_back(node->stmt.decl.name);
            break;

        // traverse nodes
        case(ast_call):
            if(node->stmt.call.param != NULL) {
                result &= semanticAnalysis(node->stmt.call.param);
            }
            break;

        // traverse nodes
        case(ast_ret):
            assert(node->stmt.ret.expr != NULL);
            result &= semanticAnalysis(node->stmt.ret.expr);
            break;

        // traverse nodes
        case(ast_while):
            assert(node->stmt.whilen.cond != NULL);
            result &= semanticAnalysis(node->stmt.whilen.cond);
            assert(node->stmt.whilen.body != NULL);
            result &= semanticAnalysis(node->stmt.whilen.body);
            break;

        // traverse nodes
        case(ast_if):
            assert(node->stmt.ifn.cond != NULL);
            result &= semanticAnalysis(node->stmt.ifn.cond);
            assert(node->stmt.ifn.if_body != NULL);
            result &= semanticAnalysis(node->stmt.ifn.if_body);
            if(node->stmt.ifn.else_body != NULL) {
                result &= semanticAnalysis(node->stmt.ifn.else_body);
            }
            break;

        // traverse nodes
        case(ast_asgn):
            assert(node->stmt.asgn.lhs != NULL);
            result &= semanticAnalysis(node->stmt.asgn.lhs);
            assert(node->stmt.asgn.rhs != NULL);
            result &= semanticAnalysis(node->stmt.asgn.rhs);
            break;

        // if a block statement, create new var list and iterate statements
        case(ast_block):
            vector<char*>* current_symbols = new vector<char*>();
            stack.push_front(current_symbols);

            vector<astNode*>* slist = node->stmt.block.stmt_list;
            vector<astNode*>::iterator it = slist->begin();
            while(it != slist->end()) { // iterate through all statements and traverse
                result &= semanticAnalysis(*it);
                it++;
            }

            // free the front symbol vector
            stack.pop_front();
            delete(current_symbols);
            break;

    }

    return result;
}

/* checks if a variable is on the symbol table 
 * that is, it checks if the var is on any of the vectors in our stack 
 */
bool onSymbolTable(char* var) { 
    assert(var != NULL);

    // iterate through all of the tables on the stack
    deque<vector<char*>*>::iterator st_it = stack.begin();
    while(st_it != stack.end()) {
        // loop through all symbols and check if any are equal to the var
        vector<char*>* symbols = *st_it;
        vector<char*>::iterator vec_it = symbols->begin();
        while(vec_it != symbols->end()) {
            assert(*vec_it != NULL);
            if(strcmp(*vec_it, var) == 0) { // if equal return true
                return true;
            }
            vec_it++;
        }

        st_it++;
    }

    return false;
}

/* OPTIMIZED SOLUTION */
/* ------------------ */

/* main semantic analysis method - 
 * traverses nodes and handles them according to type 
 * optimized using a set for faster runtime;
 * additionally throws errors in re-declarations 
 */
bool semanticAnalysis_opt(astNode* node) {  
    assert(node != NULL);

    bool result = true;
    // react based on node types
    switch(node->type) {

        // traverse nodes
        case(ast_prog):
            assert(node->prog.ext1 != NULL);
            result &= semanticAnalysis_opt(node->prog.ext1); // unnecessary
            assert(node->prog.ext2 != NULL);
            result &= semanticAnalysis_opt(node->prog.ext2); // unnecessary
            assert(node->prog.func != NULL);
            result &= semanticAnalysis_opt(node->prog.func);
            break;

        // handle block and declaration statements
        case(ast_stmt):
            result &= handleStatements_opt(node);
            break;

        // do nothing
        case(ast_extern):
            break;

        // check if the variable is on the symbol table, if not print an error
        case(ast_var):
            assert(node->var.name != NULL);
            if(!onSymbolTable_opt(node->var.name)) {
                fprintf(stdout, "Symbol error: var [%s] has not been declared\n\n", node->var.name);
                result = false;
            }
            break;

        // do nothing
        case(ast_cnst):
            break;

        // traverse nodes
        case(ast_rexpr):
            assert(node->rexpr.lhs != NULL);
            result &= semanticAnalysis_opt(node->rexpr.lhs);
            assert(node->rexpr.rhs != NULL);
            result &= semanticAnalysis_opt(node->rexpr.rhs);
            break;

        // traverse nodes
        case(ast_bexpr):
            assert(node->bexpr.lhs != NULL);
            result &= semanticAnalysis_opt(node->bexpr.lhs);
            assert(node->bexpr.rhs != NULL);
            result &= semanticAnalysis_opt(node->bexpr.rhs);
            break;

        // traverse nodes
        case(ast_uexpr):
            assert(node->uexpr.expr != NULL);
            result &= semanticAnalysis_opt(node->uexpr.expr);
            break;

        // handle parameters
        case(ast_func):
            // create new var list and add parameter if it exists
            vector<char*>* current_symbols = new vector<char*>();
            if(node->func.param != NULL) {
                stack.push_front(current_symbols);
                assert(node->func.param->var.name != NULL);
                current_symbols->push_back(node->func.param->var.name);
                activeSymbols.insert(string(node->func.param->var.name));
            }

            assert(node->func.body != NULL);
            result &= semanticAnalysis_opt(node->func.body); // traverse into body of function

            // free the front symbol vector
            stack.pop_front();
            deleteNonActiveSymbols_opt(current_symbols);
            delete(current_symbols);
            break;

    }
    
    return result;
}

/* handles all of the possible statement types 
 * optimized using a set for faster runtime 
 */
bool handleStatements_opt(astNode* node) {
    assert(node != NULL);
   
    bool result = true;
    // react based on statement types
    switch(node->stmt.type) {

        // if a declaration, add var to list in top of stack
        case(ast_decl):
            // error if already exists - duplicate declaration
            assert(node->stmt.decl.name != NULL);
            if(onFrontSymbolTable_opt(node->stmt.decl.name)) {
                fprintf(stdout, "Symbol error: var [%s] has already been declared\n\n", node->stmt.decl.name);
                result = false;
                break;
            }
            stack.front()->push_back(node->stmt.decl.name);
            activeSymbols.insert(string(node->stmt.decl.name));
            break;

        // traverse nodes
        case(ast_call):
            if(node->stmt.call.param != NULL) {
                result &= semanticAnalysis_opt(node->stmt.call.param);
            }
            break;

        // traverse nodes
        case(ast_ret):
            assert(node->stmt.ret.expr != NULL);
            result &= semanticAnalysis_opt(node->stmt.ret.expr);
            break;

        // traverse nodes
        case(ast_while):
            assert(node->stmt.whilen.cond != NULL);
            result &= semanticAnalysis_opt(node->stmt.whilen.cond);
            assert(node->stmt.whilen.body != NULL);
            result &= semanticAnalysis_opt(node->stmt.whilen.body);
            break;

        // traverse nodes
        case(ast_if):
            assert(node->stmt.ifn.cond != NULL);
            result &= semanticAnalysis_opt(node->stmt.ifn.cond);
            assert(node->stmt.ifn.if_body != NULL);
            result &= semanticAnalysis_opt(node->stmt.ifn.if_body);
            if(node->stmt.ifn.else_body != NULL) {
                result &= semanticAnalysis_opt(node->stmt.ifn.else_body);
            }
            break;

        // traverse nodes
        case(ast_asgn):
            assert(node->stmt.asgn.lhs != NULL);
            result &= semanticAnalysis_opt(node->stmt.asgn.lhs);
            assert(node->stmt.asgn.rhs != NULL);
            result &= semanticAnalysis_opt(node->stmt.asgn.rhs);
            break;

        // if a block statement, create new var list and iterate statements
        case(ast_block):
            vector<char*>* current_symbols = new vector<char*>();
            stack.push_front(current_symbols);

            vector<astNode*>* slist = node->stmt.block.stmt_list;
            vector<astNode*>::iterator it = slist->begin();
            while(it != slist->end()) { // iterate through all statements and traverse
                assert(*it != NULL);
                result &= semanticAnalysis_opt(*it);
                it++;
            }

            // free the front symbol vector
            stack.pop_front();
            deleteNonActiveSymbols_opt(current_symbols);
            delete(current_symbols);
            break;

    }

    return result;
}

/* checks if a variable is on the symbol table 
 * that is, it checks if the var is on any of the vectors in our stack 
 * optimized using a set 
 */
bool onSymbolTable_opt(string var) { 
    return (activeSymbols.find(var) != activeSymbols.end());
}

/* checks if a variable is on the top symbol table vector */
bool onFrontSymbolTable_opt(char* var) { 
    assert(var != NULL);

    // loop through all symbols on top vector and check if any are equal to the var
    vector<char*>* symbols = stack.front();
    vector<char*>::iterator it = symbols->begin();
    while(it != symbols->end()) {
        assert(*it != NULL);
        if(strcmp(*it, var) == 0) { // if equal return true
            return true;
        }
        it++;
    }

    return false;
}

/* deletes any symbols in the given symbol table from the symbol set */
void deleteNonActiveSymbols_opt(vector<char*>* symbols) {
    assert(symbols != NULL);
    vector<char*>::iterator it = symbols->begin();
    while(it != symbols->end() ) {
        assert(*it != NULL);
        activeSymbols.erase(string(*it));
        it++;
    }
}