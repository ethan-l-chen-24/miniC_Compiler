lib = ../lib/ast/ast
main = optimizer
source = optimized
folder = ../lib/test_files
subfolder = llvm_given
test_file = vasanta

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb -gdwarf-4 -g
syntax_files = ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c
llvm_builder_files = ../llvm_ir_builder/llvm_gen.c
helper_files = ../helper/helper_functions.c

build: llvm_optimizations.c $(main).c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(llvm_builder_files) $(helper_files) $(lib).c llvm_optimizations.c $(main).c

test:
	./$(source).out load $(folder)/$(subfolder)/$(test_file).ll $(folder)/llvm_optimized/$(test_file).ll

hard_test:
	clang++ $(clang_flags) -o tester.out llvm_optimizations.c $(helper_files)  optimizer_tests.c
	./tester.out $(folder)/llvm_optimized/hard_test.ll

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out build $(folder)/$(subfolder)/$(test_file).c $(folder)/llvm_optimized/$(test_file).ll

debug:
	gdb --args ./$(source).out load $(folder)/$(subfolder)/$(test_file).ll $(folder)/llvm_optimized/$(test_file).ll

clean:
	rm *.out *.o *.ll