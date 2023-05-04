lib = ../lib/ast/ast
main = optimizer
source = optimized
folder = ../lib/test_files
subfolder = files
test_file = p5

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb
syntax_files = ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c
llvm_builder_files = ../llvm_ir_builder/llvm_gen.c

build: llvm_optimizations.c $(main).c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(llvm_builder_files) $(lib).c llvm_optimizations.c $(main).c

test:
	./$(source).out $(folder)/$(subfolder)/$(test_file).c $(folder)/llvm_optimized/$(test_file).ll

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out $(folder)/$(subfolder)/$(test_file).c

debug:
	gdb --args ./$(source).out $(folder)/$(subfolder)/$(test_file).c $(folder)/llvm_optimized/$(test_file).ll

clean:
	rm *.out *.o *.ll