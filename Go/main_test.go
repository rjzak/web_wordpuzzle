package main

import (
	"strings"
	"testing"
)

// Ensure that all words are five characters long
func TestWordLength(t *testing.T) {
	wordArray := strings.Split(words, "\n")
	for index, word := range wordArray {
		if len(word) != 5 {
			t.Errorf("Word #%d %s is not five characters long!", index, word)
		}
	}
}
