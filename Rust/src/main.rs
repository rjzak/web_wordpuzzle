use std::net::SocketAddr;
use std::collections::HashMap;
use std::sync::Arc;
use axum::{
    AddExtensionLayer,
    Router,
    routing::get,
    response::Html,
    extract::{Extension, Query},
};
use rand::Rng;

struct State {
    the_word: String,
}

#[tokio::main]
async fn main() {
    const WORDLIST: &str = include_str!("../../assets/wordList.txt");

    let mut rng = rand::thread_rng();
    let mut random_index: usize = rng.gen_range(0..WORDLIST.len()/6);

    while WORDLIST.as_bytes()[random_index] != 0x0A { // check for newline
        random_index += 1;
    }
    random_index += 1;
    let mut the_word = &WORDLIST[random_index..random_index+5];
    let the_word_string = the_word.to_lowercase();
    the_word = the_word_string.as_str().trim();
    let shared_state = Arc::new(State {the_word: the_word.to_string() });
    println!("The word: {}", the_word);

    let app = Router::new()
        .route("/", get(game_board))
        .route("/jquery.js", get(jquery))
        .route("/guess", get(check_word))
        .layer(AddExtensionLayer::new(shared_state));

    let addr = SocketAddr::from(([127, 0, 0, 1], 8080));
    print!("Listening on localhost:8080\n");
    axum::Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}

async fn game_board() -> Html<&'static str> {
    Html(include_str!("../../assets/board.html"))
}

async fn jquery() -> &'static str {
    include_str!("../../assets/jquery-3.6.0.min.js")
}

async fn check_word(Extension(state): Extension<Arc<State>>, Query(params): Query<HashMap<String, String>>) -> String {
    let word_size:usize = state.the_word.len() as usize;
    let mut response = vec!('c'; word_size);
    for letter_index in 0..word_size {
        if params["word"].as_bytes()[letter_index] == state.the_word.as_bytes()[letter_index] {
            response[letter_index] = 'g';
        } else {
            for letter_byte in state.the_word.as_bytes() {
                if params["word"].as_bytes()[letter_index].eq(letter_byte) {
                    response[letter_index] = 'y';
                }
            }
        }
    }
    response.into_iter().collect()
}
