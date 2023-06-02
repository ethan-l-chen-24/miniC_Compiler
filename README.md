# Mini-C Compiler
## Ethan Chen, COSC 57, F004H1G

This repository contains the code for a compiler for mini-C, a reduced version of the C programming language. To compile a miniC file, adjust the `folder` and `test_file` variables in the Makefile to match the file you want to compile, and adjust the `target` variable to set the outputted target file name. Call `make modules` and then `make build` (or alternatively `make` to compute both targets at once) to compile the code. Finally, run `make assemble` to generate and run the assembly code, which uses the `main` function provided in `runner.c`. Provided in `lib/test_files` are multiple folders:
 - `files` contains `.c` files to be tested
 - `tokenized` contains the tokenized `.c` files from `syntax_analyzer`
 - `semantic_tests` contains files that should fail the `semantic_analysis` in the `syntax_analyzer`
 - `asts` contains the outputted abstract syntax trees from `syntax_analyzer`
 - `llvm` contains the outputted llvm module from `llvm_ir_builder`
 - `llvm_given` contains hardcoded llvm modules
 - `llvm_optimized` contains the outputted, optimized llvm module from `optimizer`
 - `assembly` contains the outputted assembly from `assembly_generator`
 
 Additionally, there is a `helper` module that contains a library of auxiliary helper functions generic to all sections of the project.

The compiler is broken up into distinct modules: 

### 1. Syntax Analyzer

The Syntax Analyzer parses the c file into individual tokens, and then analyzes the arrangement of tokens using a language grammar to generate an Abstract Syntax Tree. Finally, it performs semantic analysis to ensure all variable usages originate from a variable declaration. To test the syntax analyzer, `cd` into `syntax_analyzer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. Also provided in `lib/test_files` are two more directories, `asts` and `tokenized`. The first provides the outputted ASTs of the test files, while the second is the Lex-ified versions of the input files with the corresponding tokens.

### 2. LLVM IR Builder

The LLVM IR translates the Abstract Syntax Tree into a generic LLVM Intermediate Representation, using the LLVM-C API. To test the LLVM IR Builder, `cd` into `llvm_ir_builder` and build using `make`. In the Makefile, you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.ll` file will be outputted in the `lib/test_files/llvm` directory, allowing you to check the semantics of the LLVM IR against the original code.

### 3. Optimizer

The Optimizer takes the LLVM IR and removes any unnecessary instructions, using 4 optimization techniques:
- common subexpressions elimination
- deadcode elimination
- constant folding
- constant propagation

To test the Optimizer, `cd` into `optimizer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.ll` file will be outputted in the `lib/test_files/llvm_optimized` directory, allowing you to check the semantics of the LLVM IR against the original code. For smaller custom test cases, run `make hard_test`.

### 4. Assembly Generator

The Assembly Generator takes any LLVM IR and converts into x86 assembly instructions. To test the Assembly Generator, `cd` into `assembly_generator` and build using `make`. In the Makefile, you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.s` file will be outputted in the `lib/test_files/assembly` directory, allowing you to check and test the outputted assembly code.

### Extra Notes

This program makes a few assumptions. These assumptions are relevant so the LLVM IR can be built properly and the assembly does not return a segmentation fault. 

- all functions have a return statement
- all declared variables are also initialized
