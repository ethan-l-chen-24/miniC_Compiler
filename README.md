# Mini-C Compiler
## Ethan Chen, COSC 57, F004H1G

This repository contains the code for a compiler for mini-C, a reduced version of the C programming language. To compile a miniC file, adjust the `folder` and `test_file` variables in the Makefile to match the file you want to compile, and call `make`. Provided in `lib/test_files` are two folders: `files` and `semantic_tests` which can be used as test files for the different parts of the compiler.

The compiler is broken up into distinct sections: 

### 1. Syntax Analyzer

To test the syntax analyzer, `cd` into `syntax_analyzer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. Also provided in `lib/test_files` are two more directories, `asts` and `tokenized`. The first provides the outputted ASTs of the test files, while the second is the Lex-ified versions of the input files with the corresponding tokens.

### 2. Syntax Analyzer

To test the syntax analyzer, `cd` into `syntax_analyzer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath. By saying `make test`, a `.ll` file will be outputted in the same module, allowing you to check the semantics of the LLVM IR against the original code.