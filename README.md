# Mini-C Compiler
## Ethan Chen, COSC 57, F004H1G

This repository contains the code for a compiler for mini-C, a reduced version of the C programming language. To compile a miniC file, adjust the `folder` and `test_file` variables in the Makefile to match the file you want to compile, and call `make`.

The compiler is broken up into distinct sections: 

### 1. Syntax Analyzer

To test the syntax analyzer, `cd` into `syntax_analyzer` and build using `make`. In the Makefile, , you can adjust the test file by altering the `folder` and `test_file` variables to the proper filepath.