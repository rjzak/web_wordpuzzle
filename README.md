## Compiling
* For your local system: `go build`
* For [Enarx](https://enarx.dev/), use [TinyGo](https://tinygo.org/): `tinygo build -wasm-abi=generic -target=wasi -o main.wasm main.go`

## Requirements:
* Go version 1.16 or higher