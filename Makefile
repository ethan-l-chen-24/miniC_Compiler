.PHONY: all clean

all:
	make -C syntax_analyzer

clean:
	make -C syntax_analyzer clean