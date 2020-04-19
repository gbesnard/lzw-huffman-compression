#!/bin/bash
compute () {
    printf "\n============================\n"
    INPUT=$1
    COMPRESS=$2
    DECOMPRESS=$3
    echo "Source : $INPUT"
    SIZE_SRC=$(stat --printf="%s" $INPUT)
    echo "Compress and decompress : ../bin/huffman $INPUT $COMPRESS $DECOMPRESS"
    ../bin/huffman $INPUT $COMPRESS $DECOMPRESS
    set +x

    SIZE_CMP=$(stat --printf="%s" $COMPRESS)
    TAUX_CMP=$(bc -l <<< "scale=2; 1-$SIZE_CMP/$SIZE_SRC")
    printf "\n"
    echo "diff $DECOMPRESS $INPUT"
    diff $DECOMPRESS $INPUT
    printf "\n"
    echo "Original size : $SIZE_SRC bytes"
    echo "Compressed size : $SIZE_CMP bytes"
    echo "Compression ratio : $TAUX_CMP"
    printf "\n"
}

INPUT=../../test-input/input.txt
COMPRESS=output/compress.txt
DECOMPRESS=output/decompress.txt
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../../test-input/linput.mp3
COMPRESS=output/lcompress.mp3
DECOMPRESS=output/ldecompress.mp3
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../../test-input/input.wav
COMPRESS=output/compress.wav
DECOMPRESS=output/decompress.wav
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../../test-input/input.png
COMPRESS=output/compress.png
DECOMPRESS=output/decompress.png
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../../test-input/input.bmp
COMPRESS=output/compress.bmp
DECOMPRESS=output/decompress.bmp
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../bin/huffman
COMPRESS=output/compress.exe
DECOMPRESS=output/decompress.exe
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../../test-input/big_input.txt
COMPRESS=output/big_compress.txt
DECOMPRESS=output/big_decompress.txt
compute $INPUT $COMPRESS $DECOMPRESS
