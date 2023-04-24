source = a
folder = files
test_file = test

.PHONY: all modules compile clean

all: modules compile

modules:
	make -C syntax_analyzer

compile: main.c
	g++ -ggdb -o $(source).out syntax_analyzer/y.tab.c syntax_analyzer/lex.yy.c syntax_analyzer/semantic_analysis.c ast/ast.c main.c
	./$(source).out test_files/$(folder)/$(test_file).c

clean:
	rm $(source).out
	make -C syntax_analyzer clean