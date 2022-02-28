package main

import (
	_ "embed"
	"fmt"
	"github.com/rjzak/tinywww"
	"math/rand"
	"os"
	"strings"
	"time"
)

//go:generate cp -r ../assets assets
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

	wwwwServer, err := tinywww.NewTinyWWW("127.0.0.1:8080")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to listen on 127.0.0.1:8080: %s\n", err)
		os.Exit(1)
	}

	wwwwServer.HandleGet("/", func(resp *tinywww.TinyResponse, req *tinywww.TinyRequest) {
		resp.SetHeader("Content-Type", "text/html")
		_, err := resp.Write(gameBoard)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing gameboard client: %s\n", err)
		}
	})

	wwwwServer.HandleGet("/jquery.js", func(resp *tinywww.TinyResponse, req *tinywww.TinyRequest) {
		resp.SetHeader("Content-Type", "text/javascript")
		_, err := resp.Write(jquery)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing jquery to client: %s\n", err)
		}
	})

	wwwwServer.HandleGet("/guess", func(resp *tinywww.TinyResponse, req *tinywww.TinyRequest) {
		resp.SetHeader("Content-Type", "text/plain")
		word, okay := req.Form["word"]
		result := "ccccc"
		if okay {
			result = process_guess(theWord, word)
		}
		_, err := resp.Write([]byte(result))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing guess response to http.ResponseWriter: %s\n", err)
		}
	})

	fmt.Fprintf(os.Stderr, "Listening on 127.0.0.1:8080...\n")
	if err := wwwwServer.ListenMaybeExit(false); err != nil {
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
