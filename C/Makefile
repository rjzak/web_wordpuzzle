FLAGS=-Wall -Isrc/

ALL:web_wordpuzzle

main.o: src/main.c src/www.h
	gcc -c src/main.c ${FLAGS}

www.o: src/www.c src/www.h
	gcc -c src/www.c ${FLAGS}

web_wordpuzzle: main.o www.o
	gcc main.o www.o -o web_wordpuzzle ${FLAGS}

.phony: clean
clean:
	rm *.o web_wordpuzzle