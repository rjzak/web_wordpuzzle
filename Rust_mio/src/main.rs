//use simple_logger::SimpleLogger;
use rand::Rng;
use mini_http;
use mini_http::Server;

// All the web server and network code by Harald H.
// https://github.com/haraldh

fn jquery_js() -> &'static [u8] {
    include_bytes!("../../assets/jquery-3.6.0.min.js")
}

fn index_page() -> &'static [u8] {
    include_bytes!("../../assets/board.html")
}

const NOT_FOUND: &str = r#"
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head>
<title>404 Not Found</title>
</head><body>
<h1>Not Found</h1>
<p>The requested URL was not found on this server.</p>
</body></html>
"#;

fn check_word(query: Option<&str>, the_word: String) -> Vec<u8> {
    let mut response = vec![b'c'; 5];

    if query.is_some() {
        let mut the_guess = query.unwrap();
        let the_guess_parts = the_guess.split_once("=").unwrap();
        the_guess = the_guess_parts.1;
        let word_size:usize = the_guess.len() as usize;
        if word_size == 5 {
            for letter_index in 0..word_size {
                if the_guess.as_bytes()[letter_index] == the_word.as_bytes()[letter_index] {
                    response[letter_index] = b'g';
                } else {
                    for letter_byte in the_word.as_bytes() {
                        if the_guess.as_bytes()[letter_index].eq(letter_byte) && response[letter_index] == b'c' {
                            response[letter_index] = b'y';
                        }
                    }
                }
            }
        }
    }

    return response;
}

#[cfg(target_os = "wasi")]
fn get_server() -> Server {
    mini_http::Server::preopened().unwrap()
}

#[cfg(not(target_os = "wasi"))]
fn get_server() -> Server {
    mini_http::Server::new("127.0.0.1:8443").unwrap()
}

fn run() -> Result<(), Box<dyn std::error::Error>> {
    const WORDLIST: &str = include_str!("../../assets/wordList.txt");

    let mut rng = rand::thread_rng();
    let mut random_index: usize = rng.gen_range(0..WORDLIST.len()/6);

    while WORDLIST.as_bytes()[random_index] != 0x0A { // check for newline
        random_index += 1;
    }
    random_index += 1;
    let the_word_static = &WORDLIST[random_index..random_index+5];
    let the_word_copy: &'static str = the_word_static.clone();
    let the_word_string = the_word_copy.to_lowercase();

    println!("The word: {}", the_word_string);

    get_server()
        .tcp_nodelay(true)
        .start(move |req| match req.uri().path() {
            "/jquery.js" => mini_http::Response::builder()
                .status(200)
                .header("Content-Type", "text/javascript")
                .body(jquery_js().to_vec())
                .unwrap(),
            "/guess" => mini_http::Response::builder()
                .status(200)
                .header("Content-Type", "text/plain")
                .body(check_word(req.uri().query(), the_word_string.clone()))
                .unwrap(),
            "/" => mini_http::Response::builder()
                .status(200)
                .header("Content-Type", "text/html")
                .body(index_page().to_vec())
                .unwrap(),
            _ => mini_http::Response::builder()
                .status(404)
                .body(NOT_FOUND.as_bytes().to_vec())
                .unwrap(),
        })?;
    Ok(())
}

pub fn main() {
    if let Err(e) = run() {
        eprintln!("Error: {:?}", e);
    }
}
