#include<vector>
#include<deque>
#include<set>
#include<string>
#include "../ast/ast.h"

/* FUNCTIONS */
/* --------- */

// non-optimized methods
bool semanticAnalysis(astNode* node);
bool handleStatements(astNode* node);
bool onSymbolTable(char* var);

// optimized methods
bool semanticAnalysis_opt(astNode* node);
bool handleStatements_opt(astNode* node);
bool onSymbolTable_opt(string var);
bool onFrontSymbolTable_opt(char* var);
void deleteNonActiveSymbols_opt(vector<char*>* symbols);