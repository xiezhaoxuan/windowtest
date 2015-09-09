test:test.o usart.o
	gcc -o test test.o usart.o -mwindows
test.o:test.c
	gcc -c test.c
usart.o:usart.c
	gcc -c usart.c

clean:
	del test.exe test.o usart.o
