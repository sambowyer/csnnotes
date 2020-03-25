CFLAGS =  -Wall -O0 -std=c99
LFLAGS =

all: main 

main: main.o
	gcc $(CFLAGS) -o main main.o $(LFLAGS)

main.o: main.c
	gcc $(CFLAGS) -c -o main.o main.c 

clean:
	rm *.o
