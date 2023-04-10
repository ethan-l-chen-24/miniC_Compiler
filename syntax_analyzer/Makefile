lex_source = tokenizer
yacc_source = grammar
source = syntax
$(source).out: $(lex_source).l $(yacc_source).y
	yacc -d -v $(yacc_source).y
	lex $(lex_source).l
	gcc -o $(source).out lex.yy.c y.tab.c
clean:
	rm lex.yy.c y.tab.c y.tab.h $(source).out y.output
