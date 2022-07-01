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
	gcc main.o www.o -o web_wordpuzzle ${FLAGS} -O1

web_wordpuzzle_debug: src/main.c src/www.h src/www.c src/assets.h
	gcc src/main.c src/www.c ${FLAGS} -DDEBUG -ggdb -o web_wordpuzzle_debug

web_wordpuzzle.wasm: src/main.c src/www.c src/www.h src/assets.h
	${WASISDK}/bin/clang --sysroot=${WASISDK}/share/wasi-sysroot/ src/main.c src/www.c -Isrc/ ${FLAGS} -O1 -o web_wordpuzzle.wasm

web_wordpuzzle_debug.wasm: src/main.c src/www.c src/www.h src/assets.h
	${WASISDK}/bin/clang --sysroot=${WASISDK}/share/wasi-sysroot/ src/main.c src/www.c -Isrc/ ${FLAGS} -DDEBUG -o web_wordpuzzle_debug.wasm

run_wasm_wasmtime: web_wordpuzzle.wasm
	wasmtime run --tcplisten 127.0.0.1:8080 --env FD_COUNT=1 web_wordpuzzle.wasm

run_wasm_enarx: web_wordpuzzle.wasm
	enarx run --wasmcfgfile ../Enarx.toml web_wordpuzzle.wasm

.phony: clean
clean:
	rm -f *.o web_wordpuzzle web_wordpuzzle_debug web_wordpuzzle.wasm web_wordpuzzle_debug.wasm src/assets.h
