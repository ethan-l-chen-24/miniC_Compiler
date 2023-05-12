# Mini-C Compiler
## Ethan Chen, COSC 57, F004H1G

This repository contains the code for a compiler for mini-C, a reduced version of the C programming language. To compile a miniC file, adjust the `folder` and `test_file` variables in the Makefile to match the file you want to compile, and call `make` and then `make compile`. Provided in `lib/test_files` are two folders: `files` and `semantic_tests` which can be used as test files for the different parts of the compiler. The `helper` module contains a library of auxiliary helper functions generic to all sections of the project.

The compiler is broken up into distinct sections: 

### 1. Syntax Analyzer

The Syntax Analyzer parses the c file into individual tokens, and then analyzes the arrangement of tokens using a language grammar to generate an Abstract Syntax Tree. Finally, it performs semantic analysis to ensure all variable usages originate from a variable declaration. To test the syntax analyzer, `cd` into `syntax_analyzer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. Also provided in `lib/test_files` are two more directories, `asts` and `tokenized`. The first provides the outputted ASTs of the test files, while the second is the Lex-ified versions of the input files with the corresponding tokens.

### 2. LLVM IR Builder

The LLVM IR translates the Abstract Syntax Tree into a generic LLVM Intermediate Representation, using the LLVM-C API. To test the LLVM IR Builder, `cd` into `llvm_ir_builder` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.ll` file will be outputted in the `lib/test_files/llvm` directory, allowing you to check the semantics of the LLVM IR against the original code.

### 3. Optimizer

The Optimizer takes the LLVM IR and removes any unnecessary instructions, using 4 optimization techniques:
- common subexpressions elimination
- deadcode elimination
- constant folding
- constant propagation

To test the Optimizer, `cd` into `optimizer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.ll` file will be outputted in the `lib/test_files/llvm_optimized` directory, allowing you to check the semantics of the LLVM IR against the original code. For smaller custom test cases, run `make hard_test`.

### 4. Assembly Generator

Work in Progress
