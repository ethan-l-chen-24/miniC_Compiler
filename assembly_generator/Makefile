lib = ../lib/ast/ast
main = assembly_generator
source = assembly
folder = ../lib/test_files
subfolder = files
test_file = p2

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb -gdwarf-4 -g
syntax_files = ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c
llvm_builder_files = ../llvm_ir_builder/llvm_gen.c
optimizer_files = ../optimizer/llvm_optimizations.c
helper_files = ../helper/helper_functions.c

build: llvm_to_assembly.c $(main).c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(llvm_builder_files) $(optimizer_files) $(helper_files) $(lib).c llvm_to_assembly.c $(main).c

test:
	./$(source).out build $(folder)/$(subfolder)/$(test_file).c $(folder)/assembly/$(test_file).s

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out build $(folder)/$(subfolder)/$(test_file).c $(folder)/assembly/$(test_file).s

debug:
	gdb --args ./$(source).out build $(folder)/$(subfolder)/$(test_file).c $(folder)/assembly/$(test_file).s

clean:
	rm *.out