FLAGS=-Wall -Isrc/
WASISDK=/opt/wasi-sdk/

all:web_wordpuzzle

src/assets.h: ../assets/board.html ../assets/jquery-3.6.0.min.js  ../assets/wordList.txt
	python3 process_assets.py

main.o: src/main.c src/www.h
	gcc -c src/main.c ${FLAGS}

www.o: src/www.c src/www.h src/assets.h
	gcc -c src/www.c ${FLAGS}

web_wordpuzzle: main.o www.o
	gcc main.o www.o -o web_wordpuzzle ${FLAGS}

web_wordpuzzle.wasm: src/main.c src/www.c src/www.h
	${WASISDK}/bin/clang --sysroot=${WASISDK}/share/wasi-sysroot/ src/main.c src/www.c -Isrc/ -DDEBUG -o web_wordpuzzle.wasm

run_wasm: web_wordpuzzle.wasm
	wasmtime run --tcplisten 127.0.0.1:8080 --env FD_COUNT=1 web_wordpuzzle.wasm

.phony: clean
clean:
	rm *.o web_wordpuzzle web_wordpuzzle.wasm
