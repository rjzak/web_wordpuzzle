## C version

### Compiling
* In the C directory, simply run `make`.
* Python 3 is required, as it's used to make C arrays from the assets for embedding in the game.
* Wasm target required the [wasi-sdk](https://github.com/WebAssembly/wasi-sdk/releases), but doesn't yet work.

### Requirements
* gcc or llvm

### Acknowledgements
Thanks to [Dungyichao](https://github.com/Dungyichao/http_server) for the beginner code which was the basis for working with sockets.