test:test.o
	gcc -o test test.o -mwindows
test.o:test.c
	gcc -c test.c

clean:
	del test.exe test.o
