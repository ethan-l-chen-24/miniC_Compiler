#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include <unordered_map>
#include <set>
#include <array>

using namespace std;

/* FUNCTIONS */
/* --------- */

array<unordered_map<LLVMBasicBlockRef, set<LLVMBasicBlockRef>>, 2> generateGraphs(LLVMValueRef func);