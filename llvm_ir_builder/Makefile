lib = ../lib/ast/ast
main = llvm_ir_builder
source = ir_builder
folder = ../lib/test_files
subfolder = files
test_file = test

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb
syntax_files = ../syntax_analyzer/semantic_analysis.c ../syntax_analyzer/y.tab.c ../syntax_analyzer/lex.yy.c

build: llvm_gen.c $(main).c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(lib).c llvm_gen.c  $(main).c

test:
	./$(source).out $(folder)/$(subfolder)/$(test_file).c $(folder)/llvm/$(test_file).ll

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out $(folder)/$(subfolder)/$(test_file).c

debug:
	gdb --args ./$(source).out $(folder)/$(subfolder)/$(test_file).c

clean:
	rm *.out *.o *.ll