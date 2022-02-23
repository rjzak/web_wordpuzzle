package main

import (
	_ "embed"
	"fmt"
	"math/rand"
	"net/http"
	"os"
	"strings"
	"time"
)

//go:embed assets/board.html
var gameBoard []byte

//go:embed assets/jquery-3.6.0.min.js
var jquery []byte

//go:embed assets/wordList.txt
var words string

func main() {
	rand.Seed(time.Now().Unix())
	wordArray := strings.Split(words, "\n")
	randIndex := rand.Intn(len(wordArray))
	theWord := strings.ToLower(wordArray[randIndex])
	fmt.Printf("Picked word %s from list of %d words.\n", theWord, len(wordArray))
	mux := http.NewServeMux()

	mux.HandleFunc("/", func(w http.ResponseWriter, req *http.Request) {
		w.Header().Set("Content-Type", "text/html")
		_, err := w.Write(gameBoard)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing gameboard to http.ResponseWriter: %s\n", err)
		}
	})

	mux.HandleFunc("/jquery.js", func(w http.ResponseWriter, req *http.Request) {
		w.Header().Set("Content-Type", "text/javascript")
		_, err := w.Write(jquery)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing jquery to http.ResponseWriter: %s\n", err)
		}
	})

	mux.HandleFunc("/guess", func(w http.ResponseWriter, req *http.Request) {
		w.Header().Set("Content-Type", "text/plain")
		var word string = ""
		if err := req.ParseForm(); err == nil {
			word = req.Form.Get("word")
		}
		_, err := w.Write([]byte(process_guess(theWord, word)))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing guess response to http.ResponseWriter: %s\n", err)
		}
	})

	fmt.Fprintf(os.Stderr, "Listening on 127.0.0.1:8080...\n")
	if err := http.ListenAndServe("127.0.0.1:8080", mux); err != nil {
		fmt.Fprintf(os.Stderr, "HTTP Server error: %s\n", err)
	}
}

func process_guess(theWord, theGuess string) string {
	if len(theGuess) != 5 {
		return "error:length"
	}
	resp := ""
	for i := 0; i < 5; i++ {
		if theWord[i] == theGuess[i] {
			resp += "g"
		} else {
			if strings.Contains(theWord, string(theGuess[i])) {
				resp += "y"
			} else {
				resp += "c"
			}
		}
	}
	return resp
}
