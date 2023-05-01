#include<vector>
#include<deque>
#include<set>
#include<string>
#include "../lib/ast/ast.h"

/* FUNCTIONS */
/* --------- */

// non-optimized methods
bool semanticAnalysis(astNode* node);

// optimized methods
bool semanticAnalysis_opt(astNode* node);
