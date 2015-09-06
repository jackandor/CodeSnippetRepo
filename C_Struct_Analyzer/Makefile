analyzer: main.o table.o y.tab.o lex.yy.o
	cc $^ -o $@

lex.yy.o: lex.yy.c
	cc -o $@ -c $<

lex.yy.c: lex.l
	flex $<

y.tab.o: y.tab.c y.tab.h
	cc -o $@ -c $<

y.tab.c y.tab.h: grammar.y
	yacc -vd $<

table.o: table.c table.h
	cc -o $@ -c $<

main.o: main.c table.h y.tab.h
	cc -o $@ -c $<

clean:
	rm -rf *.o lex.yy.c y.tab.c y.tab.h y.output
