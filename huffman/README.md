# Huffman compression algorithm

Implementation of the huffman coding compression algorithm.

## Compile

The `Makefile` is in the huffman root folder.

```
make
```

## Usage

The binary `huffman` is in the `bin` folder.

```
./huffman input_filename output_filename
```

For now, the program compress input_filename to a comp.bin file, and decompress it directly.

Check decompressed file integrity:
```
diff input decompress
```

## Tests

Automated `test.sh` script is in the `test` folder.


## Improvements

- Large file : malloc by chunk ? (ugly big malloc for now).
- Iterative tree search ? Will introduce other malloc pb.
- Write huffman tree into the compressed file, so that it can be read by a second independant execution.
- Use same API as lzw software, and test on the same test files.
- Try to compress lzw output dictionary with a huffman second pass.
