make: compiler

lexer: lexer.l
	flex lexer.l

parser: bison.y
	bison -t -d bison.y

compiler: lexer parser
	g++ -std=c++11 -o compiler lex.yy.c bison.tab.c compiler.cpp -lfl
	make clean

debug: compiler
	./compiler in.imp out.mr
	../labor4/maszyna_rejestrowa/maszyna-rejestrowa out.mr

test: compiler
	./compiler < ../tests/test83 > out.gl 
	../labor4/maszyna_rejestrowa/maszyna-rejestrowa out.gl

testWS: compiler
	./compiler in.imp out.mr
	../labor4/maszyna_rejestrowa/maszyna-rejestrowa-cln out.mr

clean:
	rm -f lex.yy.c bison.tab.c bison.tab.h
