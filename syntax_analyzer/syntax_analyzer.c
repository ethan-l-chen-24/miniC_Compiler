#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<deque>
#include "ast/ast.h"
using namespace std;

/* EXTERNS */
/* ------- */

extern void yyparse();
extern int yylex_destroy();
extern FILE *yyin;
extern char* yytext;

/* GLOBAL VARS */
/* ----------- */

astNode* root;
deque<vector<char*>*> stack;

/* FUNCTION PROTOTYPES */
/* ------------------- */
bool semanticAnalysis(astNode* node);
bool onSymbolTable(char* var);
bool handleStatements(astNode* node);

/* METHODS */
/* ------- */

// entrypoint to the program
int main(int argc, char** argv){
	if (argc == 2){
		yyin = fopen(argv[1], "r");
	}

    // generate the AST
	yyparse();
    printf("AST:\n--------------\n");
    printNode(root);
    printf("\nRESULT:\n---------\n");

    // check semantics of the program
    bool valid_semantics = semanticAnalysis(root);
    freeNode(root);

    // close
	if (yyin != stdin)
		fclose(yyin);
	yylex_destroy();
	
    // return if failure
    if(!valid_semantics) return 1;

    printf("Looks good to me!\n\n");
	return 0;
}

/* main semantic analysis method - 
 * traverses nodes and handles them according to type */
bool semanticAnalysis(astNode* node) {  

    bool result = true;
    // react based on node types
    switch(node->type) {

        // traverse nodes
        case(ast_prog):
            result &= semanticAnalysis(node->prog.ext1) // unnecessary
                && semanticAnalysis(node->prog.ext2) // unnecessary
                && semanticAnalysis(node->prog.func);
            break;

        // handle block and declaration statements
        case(ast_stmt):
            handleStatements(node);
            break;

        // do nothing
        case(ast_extern):
            break;

        // check if the variable is on the symbol table, if not print an error
        case(ast_var):
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
            result &= semanticAnalysis(node->rexpr.lhs)
                && semanticAnalysis(node->rexpr.rhs);
            break;

        // traverse nodes
        case(ast_bexpr):
            result &= semanticAnalysis(node->bexpr.lhs)
                && semanticAnalysis(node->bexpr.rhs);
            break;

        // traverse nodes
        case(ast_uexpr):
            result &= semanticAnalysis(node->uexpr.expr);
            break;

        // handle parameters
        case(ast_func):
            // create new var list and add parameter if it exists
            if(node->func.param != NULL) {
                stack.push_front(new vector<char*>);
                stack.front()->push_back(node->func.param->var.name);
            }

            result = semanticAnalysis(node->func.body); // traverse into body of function

            // free the node
            vector<char*>* front = stack.front();
            stack.pop_front();
            free(front);
            break;

    }
    
    return result;
}

/* checks if a variable is on the symbol table 
 * that is, it checks if the var is on any of the vectors in our stack */
bool onSymbolTable(char* var) { 

    // iterate through all of the tables on the stack
    deque<vector<char*>*>::iterator st_it = stack.begin();
    while(st_it != stack.end()) {
        // loop through all symbols and check if any are equal to the var
        vector<char*>* symbols = *st_it;
        vector<char*>::iterator vec_it = symbols->begin();
        while(vec_it != symbols->end()) {
            if(strcmp(*vec_it, var) == 0) { // if equal return true
                return true;
            }
            vec_it++;
        }

        st_it++;
    }

    return false;
}

/* handles all of the possible statement types */
bool handleStatements(astNode* node) {
   
    bool result = true;
    // react based on statement types
    switch(node->stmt.type) {

        // if a declaration, add var to list in top of stack
        case(ast_decl):
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
            result &= semanticAnalysis(node->stmt.ret.expr);
            break;

        // traverse nodes
        case(ast_while):
            result &= semanticAnalysis(node->stmt.whilen.cond)
                && semanticAnalysis(node->stmt.whilen.body);
            break;

        // traverse nodes
        case(ast_if):
            result &= semanticAnalysis(node->stmt.ifn.cond) 
                && semanticAnalysis(node->stmt.ifn.if_body);
            if(node->stmt.ifn.else_body != NULL) {
                result &= semanticAnalysis(node->stmt.ifn.else_body);
            }
            break;

        // traverse nodes
        case(ast_asgn):
            result &= semanticAnalysis(node->stmt.asgn.lhs)
                && semanticAnalysis(node->stmt.asgn.rhs);
            break;

        // if a block statement, create new var list and iterate statements
        case(ast_block):
            stack.push_front(new vector<char*>);

            vector<astNode*>* slist = node->stmt.block.stmt_list;
            vector<astNode*>::iterator it = slist->begin();
            while(it != slist->end()) { // iterate through all statements and traverse
                result &= semanticAnalysis(*it);
                it++;
            }
            vector<char*>* front = stack.front();
            stack.pop_front();
            free(front);
            break;

    }

    return result;
}