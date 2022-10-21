package net.wordpuzzle;

import net.wordpuzzle.wasi.FDSocket;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class WasiPuzzleServer implements AutoCloseable {
    private final FDSocket fdsocket;
    private final WordPuzzle puzzle = new WordPuzzle();

    public WasiPuzzleServer(int fd) {
        fdsocket = new FDSocket(fd);
    }

    public void start() {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(fdsocket.getInputStream()));
            BufferedOutputStream dataOut = new BufferedOutputStream(fdsocket.getOutputStream());
            String input = in.readLine();
            System.out.println("Input received: " + input);
            dataOut.write("Hello!!".getBytes());
            dataOut.flush();

            in.close();
            dataOut.close();
        } catch (IOException ex) {
            System.err.println("Server error : " + ex);
        }
    }

    @Override
    public void close() throws Exception {
        fdsocket.close();
    }

    public static void main(String[] args) {
        WasiPuzzleServer ps = new WasiPuzzleServer(3);
        ps.start();
    }
}
