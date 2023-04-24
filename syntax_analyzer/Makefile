lex_source = tokenizer
yacc_source = grammar
lib = ../lib/ast/ast
main = syntax_analyzer
source = syntax
folder = ../lib/test_files/semantic_tests
test_file = p1
	
build: $(yacc_source).y $(lex_source).l $(lib).c semantic_analysis.c $(main).c
	yacc -d -v $(yacc_source).y
	lex $(lex_source).l
	g++ -ggdb -o $(source).out y.tab.c lex.yy.c $(lib).c semantic_analysis.c $(main).c

clean:
	rm lex.yy.c y.tab.c y.tab.h *.out y.output

test:
	./$(source).out $(folder)/$(test_file).c

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out $(folder)/$(test_file).c

debug:
	gdb --args ./$(source).out $(folder)/$(test_file).c

tree:
	./$(source).out $(folder)/$(test_file).c > asts/$(test_file).txt

tokens:
	lex $(lex_source)_output.l
	gcc -o $(source)_output.out lex.yy.c
	./$(source)_output.out < $(folder)/$(test_file).c > tokenized/$(test_file).txt
