# Java version of the word puzzle

## Compiling:
1. Make sure you have Java 11 installed (the bundled TeaVM doesn't seem to work on newer versions).
2. Make sure you have Maven installed (or that your Java IDE has it bundled already, like Netbeans, for example).
3. Run some commands:
   1. `git submodule update --init` to get the TeaVM submodule
   2. `mvn install`
   3. Run the generated .wasm file at `target/generated/wasm/teavm-wasm/classes.wasm`
   4. Run the code with Java: `java -cp target/classes/ net.wordpuzzle.WordPuzzle`
