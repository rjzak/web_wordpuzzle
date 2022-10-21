package net.wordpuzzle;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class WordPuzzleTest {

    @Test
    public void TestWordSize() {
        WordPuzzle wp = new WordPuzzle();
        assertEquals(5, wp.theWord.length());
    }

    @Test
    public void TestCorrectWordResponse() {
        WordPuzzle wp = new WordPuzzle();
        assertEquals("ggggg", wp.testWord(wp.theWord));
    }

    @Test
    public void TestIncorrectWordResponse() {
        WordPuzzle wp = new WordPuzzle();
        assertEquals("ccccc", wp.testWord("zzzzz"));
    }

    @Test
    public void TestPartialCorrectWordResponse() {
        WordPuzzle wp = new WordPuzzle();
        String test = wp.theWord.substring(3,5) + wp.theWord.substring(0, 3);
        assertNotEquals("ggggg", wp.testWord(test));
    }

    @Test
    public void TestLargeWordResponse() {
        WordPuzzle wp = new WordPuzzle();
        assertEquals("ccccc", wp.testWord(wp.theWord + "aaaaaaaaaa"));
    }
}
