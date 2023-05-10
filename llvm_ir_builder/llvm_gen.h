#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../lib/ast/ast.h"
#include <unordered_map>
#include <set>
#include <array>

/* FUNCTIONS */
/* --------- */

LLVMModuleRef createLLVMModelFromAST(astNode* root);

void optimizeLLVMBasicBlocks(LLVMModuleRef mod);

array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2> generateGraphs(LLVMValueRef func);
