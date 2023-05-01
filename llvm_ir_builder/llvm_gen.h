#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../lib/ast/ast.h"

/* FUNCTIONS */
/* --------- */

LLVMModuleRef createLLVMModelFromAST(astNode* root);