lib = ../lib/ast/ast
main = optimizer
source = optimized
folder = ../lib/test_files
subfolder = files
test_file = p1

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb
syntax_files = ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c
llvm_builder_files = ../llvm_ir_builder/llvm_gen.c

build: llvm_optimizations.c $(main).c
	clang++ $(clang_flags) $(syntax_files) $(llvm_builder_files) $(lib).c llvm_optimizations.c $(main).c

test:
	./$(source).out $(folder)/$(subfolder)/$(test_file).c $(folder)/llvm_optimized/$(test_file).ll

debug:
	gdb --args ./$(source).out $(folder)/$(test_file).clang

clean:
	rm *.out *.o *.ll