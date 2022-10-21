package net.wordpuzzle;

import net.wordpuzzle.assets.Board;
import net.wordpuzzle.assets.Jquery;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.StringTokenizer;

/**
 *
 * @author rjzak
 */
public class PuzzleServer implements Runnable {
    private final static String JS_TYPE = "text/javascript";
    private final static String HTML_TYPE = "text/html";
    private final static String PLAIN_TYPE = "text/plain";
    private final static String HTTP_OK = "200 OK";
    private final static String HTTP_NOT_FOUND = "404 NOT FOUND";
    private final int port;
    private Socket connect = null;
    private static WordPuzzle puzzle = new WordPuzzle();
    private static final boolean verbose = true;

    /**
     *
     * @param port port number used to listen for connections
     */
    public PuzzleServer(int port) {
        this.port = port;
    }

    /**
     *
     * @param c socket used for responding to requests
     */
    private PuzzleServer(Socket c) {
        connect = c;
        port = -1;
    }

    public void start() {
        // Adapted from https://ssaurel.medium.com/create-a-simple-http-web-server-in-java-3fc12b29d5fd
        try(ServerSocket serverConnect = new ServerSocket(port)) {
            System.out.println("Server started.\nListening for connections on port : " + port + " ...\n");

            // we listen until user halts server execution
            while (true) {
                PuzzleServer myServer = new PuzzleServer(serverConnect.accept());

                if (verbose) {
                    System.out.println("Connection opened.");
                }

                // create dedicated thread to manage the client connection
                Thread thread = new Thread(myServer);
                thread.start();
            }

        } catch (IOException e) {
            System.err.println("Server Connection error: " + e.getMessage());
        }
    }

    @Override
    public void run() {
        // we read characters from the client via input stream on the socket
        // we get character output stream to client (for headers)
        // get binary output stream to client (for requested data)
        try(BufferedReader in = new BufferedReader(new InputStreamReader(connect.getInputStream()));
            PrintWriter out = new PrintWriter(connect.getOutputStream());
            BufferedOutputStream dataOut = new BufferedOutputStream(connect.getOutputStream());) {

            // get first line of the request from the client
            String input = in.readLine();
            // we parse the request with a string tokenizer
            StringTokenizer parse = new StringTokenizer(input);
            String method = parse.nextToken().toUpperCase(); // we get the HTTP method of the client
            // we get file requested
            String fileRequested = parse.nextToken().toLowerCase();

            // we support only GET and HEAD methods, we check
            if (!method.equals("GET")) {
                if (verbose) {
                    System.out.println("501 Not Implemented : " + method + " method.");
                }

                String message = "Not Implemented.";
                out.println("HTTP/1.1 501 Not Implemented");
                out.println("Server: Java HTTP Server from SSaurel : 1.0");
                out.println("Content-type: text/plain");
                out.println("Content-length: " + message.length());
                out.println();
                out.flush();
                dataOut.write(message.getBytes());
                dataOut.flush();
            } else {
                byte[] data = new byte[]{};
                String type = PLAIN_TYPE;
                String status = HTTP_NOT_FOUND;
                if (fileRequested.endsWith("/") || fileRequested.endsWith("board.html") || fileRequested.endsWith("index.html")) {
                    data = Board.BOARD;
                    type = HTML_TYPE;
                    status = HTTP_OK;
                } else if (fileRequested.endsWith("jquery.js")) {
                    data = Jquery.jquery();
                    type = JS_TYPE;
                    status = HTTP_OK;
                } else if (fileRequested.contains("guess?word=")) {
                    String guessed = fileRequested.split("=")[1];
                    data = puzzle.testWord(guessed).getBytes();
                    status = HTTP_OK;
                } else {
                    data = "Not found".getBytes();
                }

                out.println("HTTP/1.1 " + status);
                out.println("Server: Java HTTP Server from SSaurel : 1.0");
                out.println("Content-type: " + type);
                out.println("Content-length: " + data.length);
                out.println();
                out.flush();

                dataOut.write(data);
                dataOut.flush();
                if (verbose) {
                    System.out.println("File " + fileRequested + " of type " + type + " returned");
                }
            }
        } catch (IOException ioe) {
            System.err.println("Server error : " + ioe);
        }
    }

    public static void main(String[] args) {
        PuzzleServer ps = new PuzzleServer(8080);
        ps.start();
    }
}