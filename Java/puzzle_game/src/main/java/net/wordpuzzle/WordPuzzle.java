package net.wordpuzzle;

import net.wordpuzzle.assets.Wordlist;

import java.util.Random;

/**
 *
 * @author rjzak
 */
public class WordPuzzle {

    protected final String theWord;
    
    public WordPuzzle() {
        Random r = new Random();
        
        int wordlistSize = Wordlist.WORDLIST.length / Wordlist.WORD_SIZE + 1;
        int randomIndex = r.nextInt(wordlistSize);
        
        while (Wordlist.WORDLIST[randomIndex] != Wordlist.DELIMITER) {
            randomIndex++;
        }
        
        theWord = String.copyValueOf(Wordlist.WORDLIST, randomIndex+1, Wordlist.WORD_SIZE);
        System.out.println(theWord);
    }

    public String testWord(String testWord) {
        String result = "ccccc";
        if (testWord.length() != theWord.length())
            return result;

        for(int i = 0; i < theWord.length(); i++) {
            if (theWord.charAt(i) == testWord.charAt(i)) {
                result = result.substring(0, i) + "g" + result.substring(i + 1);
            } else if (theWord.contains(String.valueOf(testWord.charAt(i))) && result.charAt(i) == 'c') {
                result = result.substring(0, i) + "y" + result.substring(i + 1);
            }
        }

        return result;
    }

}
