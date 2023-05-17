/*
 * Library that contains methods to turn LLVM IR into assembly
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llvm_optimizations.h"
#include "../helper/helper_functions.h"
#include <unordered_map>
#include <vector>
#include <set>
#include <array>
//#define NDEBUG
#include <cassert>

using namespace std;

/* FUNCTION PROTOTYPES */
/* ------------------- */
void computeLiveness(LLVMBasicBlockRef bb);
void sortList();
void registerAllocation(LLVMModuleRef mod);
LLVMValueRef find_spill();

void createBBLabels();
void printDirectives();
void printFunctionEnd();
void getOffsetMap();


/* GLOBAL VARIABLES */
/* ---------------- */
unordered_map<int, LLVMValueRef> instIndex;
unordered_map<LLVMValueRef, array<int, 2>> liveRange;
vector<LLVMValueRef> sortedList;
unordered_map<LLVMValueRef, int> regMap; // TODO figure out register return type


/* FUNCTIONS */
/* --------- */

/*
*/
void codegen(LLVMModuleRef mod) {

}
