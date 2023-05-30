#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../lib/ast/ast.h"
#include <unordered_map>
#include <set>
#include <array>

/* FUNCTIONS */
/* --------- */

LLVMModuleRef createLLVMModelFromAST(astNode* root, char* filename);

void optimizeLLVMBasicBlocks(LLVMModuleRef mod);
