
all: programa Memoria disco CPU
	sudo mv programa Memoria disco CPU /usr/local/bin

programa: programa.o
	gcc programa.o -o programa -Wall

Memoria: Memoria.o
	gcc Memoria.o -o Memoria -Wall

disco: disco.o
	gcc disco.o -o disco -Wall

CPU: CPU.o
	gcc CPU.o -o CPU -Wall

programa.o: programa.c
	gcc -c programa.c

Memoria.o: Memoria.c
	gcc -c Memoria.c

disco.o: disco.c
	gcc -c disco.c

CPU.o: CPU.c
	gcc -c CPU.c

clean:
	rm -f *.o programa Memoria disco CPU


