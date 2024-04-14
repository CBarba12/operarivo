
all: principal Memoria disco CPU
	sudo mv principal Memoria disco CPU /usr/local/bin

principal: principal.o
	gcc principal.o -o principal -Wall

Memoria: Memoria.o
	gcc Memoria.o -o Memoria -Wall

disco: disco.o
	gcc disco.o -o disco -Wall

CPU: CPU.o
	gcc CPU.o -o CPU -Wall

principal.o: principal.c
	gcc -c principal.c

Memoria.o: Memoria.c
	gcc -c Memoria.c

disco.o: disco.c
	gcc -c disco.c

CPU.o: CPU.c
	gcc -c CPU.c

clean:
	rm -f *.o principal Memoria disco CPU


