default: bin/test

bin/ulib.o: ulib.c ulib.h
	gcc -c ulib.c -o bin/ulib.o

bin/test: test.c bin/ulib.o
	gcc -g test.c bin/ulib.o -o bin/test
