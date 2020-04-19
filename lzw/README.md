# LZW compression algorithm

Implementation of the Lempel–Ziv–Welch (LZW) universal lossless data compression algorithm.

## Compile

The `Makefile` is in the lzw root folder.

```
make
```

Add `$(PP)` in `CFLAGS` for debug information


## Usage

The binary `lzw` is in the `bin` folder.

```
./lzw 
Usage : -c to compress
Usage : -d to decompress
Optional : filename (default stdin)
```

Usage example using standard input and output:
```
echo toto | ./lzw -c | ./lzw -d
```

Usage example using files as input:
```
./lzw -c input > compress
./lzw -d compress > decompress
```

Check decompressed file integrity:
```
diff input decompress
```


## Tests

Automated `test.sh` script is in the `test` folder.


## Design
- Compression and decompression are computed at binary level.
- Dictionary is a simple sequence array.
- A sequence is composed of a current char and a prefix pointer to another sequence.
- Dictionnary reset happens once 4096 bytes maximum size is reached.
- In addition to the ASCII characters, a `LZW_EOF` instruction is added at initialization in the dictionary to detect end of file.


## Improvements

- Change structure type to improve search time on large dictionary.
- Negative compression ratio observed for already compressed file such as MP3 or PNG:
	- Try to add multi-pass compression. 
	- Try to find the optimum for dictionary reset before divergence.
