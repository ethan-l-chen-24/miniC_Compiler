lib = ../lib/ast/ast
main = llvm_ir_builder
source = ir_builder
folder = ../lib/test_files/files
test_file = test

build: llvm_gen.c llvm_ir_builder.c
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb -o $(source).out ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c llvm_gen.c $(lib).c $(main).c

test:
	./$(source).out $(folder)/$(test_file).c

debug:
	gdb --args ./$(source).out $(folder)/$(test_file).c

clean:
	rm *.out *.o *.ll