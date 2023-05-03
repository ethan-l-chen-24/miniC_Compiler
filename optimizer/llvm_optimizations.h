#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "../lib/ast/ast.h"
#include <stdbool.h>

/* FUNCTIONS */
/* --------- */

bool commonSubexpressionElimination(LLVMModuleRef mod);
bool deadCodeElimination(LLVMModuleRef mod);
bool constantFolding(LLVMModuleRef mod);
bool constantPropagation(LLVMModuleRef mod);
