#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include <stdbool.h>

/* FUNCTIONS */
/* --------- */

void optimizeLLVM(LLVMModuleRef mod);
bool commonSubexpressionElimination(LLVMBasicBlockRef bb);
bool deadCodeElimination(LLVMBasicBlockRef bb);
bool constantFolding(LLVMBasicBlockRef bb);
bool constantPropagation(LLVMValueRef func);
