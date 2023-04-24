lex_source = tokenizer
yacc_source = grammar
lib = ast/ast
main = syntax_analyzer
source = syntax
folder = files
test_file = test

all: build test
	
build: $(yacc_source).y $(lex_source).l $(lib).c $(main).c
	yacc -d -v $(yacc_source).y
	lex $(lex_source).l
	g++ -ggdb -o $(source).out y.tab.c lex.yy.c $(lib).c $(main).c

clean:
	rm lex.yy.c y.tab.c y.tab.h *.out y.output

test:
	./$(source).out ../test_files/$(folder)/$(test_file).c

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out ../test_files/$(folder)/$(test_file).c

debug:
	gdb --args ./$(source).out ../test_files/$(folder)/$(test_file).c

tree:
	./$(source).out ../test_files/$(folder)/$(test_file).c > ../test_files/asts/$(test_file).txt

tokens:
	lex $(lex_source)_output.l
	gcc -o $(source)_output.out lex.yy.c
	./$(source)_output.out < ../test_files/$(folder)/$(test_file).c > ../test_files/tokenized/$(test_file).txt
