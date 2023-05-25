source = a
folder = lib/test_files/files
test_file = test

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb -gdwarf-4 -g
syntax_files = syntax_analyzer/semantic_analysis.c syntax_analyzer/y.tab.c syntax_analyzer/lex.yy.c
llvm_builder_files = llvm_ir_builder/llvm_gen.c
optimizer_files = optimizer/llvm_optimizations.c
assembly_gen_files = assembly_generator/llvm_to_assembly.c
helper_files = helper/helper_functions.c
lib = lib/ast/ast

.PHONY: all modules compile clean

all: modules compile

modules:
	make -C syntax_analyzer
	make -C llvm_ir_builder 
	make -C optimizer 
	make -C assembly_generator 

compile: main.c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(llvm_builder_files) $(optimizer_files) $(assembly_gen_files) $(helper_files) $(lib).c main.c
	./$(source).out $(folder)/$(test_file).c

clean:
	make -C syntax_analyzer clean
	make -C llvm_ir_builder clean
	make -C optimizer clean
	make -C assembly_generator clean
	rm $(source).out