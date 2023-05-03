lex_source = tokenizer
yacc_source = grammar
lib = ../lib/ast/ast
main = syntax_analyzer
source = syntax
folder = ../lib/test_files
subfolder = files
test_file = p1
	
build: $(yacc_source).y $(lex_source).l $(lib).c semantic_analysis.c $(main).c
	yacc -d -v $(yacc_source).y
	lex $(lex_source).l
	g++ -ggdb -o $(source).out y.tab.c lex.yy.c $(lib).c semantic_analysis.c $(main).c

clean:
	rm *.yy.c *.tab.c *.tab.h *.out y.output

test:
	./$(source).out $(folder)/$(subfolder)/$(test_file).c

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(source).out $(folder)/$(subfolder)/$(test_file).c

debug:
	gdb --args ./$(source).out $(folder)/$(subfolder)/$(test_file).c

tree:
	./$(source).out $(folder)/$(subfolder)/$(test_file).c > $(folder)/asts/$(test_file).txt

tokens:
	lex $(lex_source)_output.l
	gcc -o $(source)_output.out lex.yy.c
	./$(source)_output.out < $(folder)/$(subfolder)/$(test_file).c > $(folder)/tokenized/$(test_file).txt
