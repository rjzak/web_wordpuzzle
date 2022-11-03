FLAGS=-Wall -Isrc/ -O1
WASISDK=/opt/wasi-sdk/

all:web_wordpuzzle

src/assets.h: ../assets/board.html ../assets/jquery-3.6.0.min.js ../assets/wordList.txt
	python3 ../C/process_assets.py

main.o: src/main.cpp src/Server.h src/assets.h
	g++ -c src/main.cpp ${FLAGS}

Server.o: src/Server.cpp src/Server.h src/WordGame.h
	g++ -c src/Server.cpp ${FLAGS}

WordGame.o: src/WordGame.cpp src/WordGame.h src/assets.h
	g++ -c src/WordGame.cpp ${FLAGS}

web_wordpuzzle: main.o Server.o
	g++ main.o Server.o WordGame.o -o web_wordpuzzle ${FLAGS}
