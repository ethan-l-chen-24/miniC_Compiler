source = main
folder = lib/test_files
subfolder = files
test_file = p1
target = out

clang_flags = `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ -ggdb -gdwarf-4 -g
syntax_files = syntax_analyzer/semantic_analysis.c syntax_analyzer/y.tab.c syntax_analyzer/lex.yy.c
llvm_builder_files = llvm_ir_builder/llvm_gen.c
optimizer_files = optimizer/llvm_optimizations.c
assembly_gen_files = assembly_generator/llvm_to_assembly.c
helper_files = helper/helper_functions.c
lib = lib/ast/ast

.PHONY: all modules build clean

all: modules build

modules:
	make -C syntax_analyzer
	make -C llvm_ir_builder 
	make -C optimizer 
	make -C assembly_generator 

build: main.c
	clang++ $(clang_flags) -o $(source).out $(syntax_files) $(llvm_builder_files) $(optimizer_files) $(assembly_gen_files) $(helper_files) $(lib).c main.c
	./$(source).out $(folder)/$(subfolder)/$(test_file).c $(test_file).s

assemble: runner.c
	as -f -32 $(test_file).s -o $(test_file).o
	gcc -m32 runner.c $(test_file).o
	./a.out

clean:
	make -C syntax_analyzer clean
	make -C llvm_ir_builder clean
	make -C optimizer clean
	make -C assembly_generator clean
	rm $(source).out *.o