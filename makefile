bugman.o: main.o pac.o
	  gcc -o bugman.o main.o pac.o -lpthread -lncurses

main.o: main.c pac.h
	  gcc -c main.c -o main.o -lpthread -lncurses

pac.o: pac.c pac.h
	  gcc -c pac.c -o pac.o -lpthread -lncurses

clean:
	  rm *.o

run:
	  ./bugman.o
