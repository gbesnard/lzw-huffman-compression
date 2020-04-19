#!/bin/bash
compute () {
    printf "\n============================\n"
    INPUT=$1
    COMPRESS=$2
    DECOMPRESS=$3
    echo "Source : $INPUT"
    SIZE_SRC=$(stat --printf="%s" $INPUT)
    echo "Compress : ../bin/lzw -c $INPUT > $COMPRESS"
    ../bin/lzw -c $INPUT > $COMPRESS
    set +x
    echo "Decompress : ../bin/lzw -d $COMPRESS > $DECOMPRESS"
    ../bin/lzw -d $COMPRESS > $DECOMPRESS 

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

printf "\n============================\n"
printf "Input redirection : echo toto | ../bin/lzw -c | ../bin/lzw -d\n"
echo toto | ../bin/lzw -c | ../bin/lzw -d

printf "\nPress enter ..."
read 

INPUT=../test/input/input.txt
COMPRESS=../test/output/compress.txt
DECOMPRESS=../test/output/decompress.txt
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../test/input/linput.mp3
COMPRESS=../test/output/lcompress.mp3
DECOMPRESS=../test/output/ldecompress.mp3
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../test/input/input.wav
COMPRESS=../test/output/compress.wav
DECOMPRESS=../test/output/decompress.wav
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../test/input/input.png
COMPRESS=../test/output/compress.png
DECOMPRESS=../test/output/decompress.png
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../test/input/input.bmp
COMPRESS=../test/output/compress.bmp
DECOMPRESS=../test/output/decompress.bmp
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../bin/lzw
COMPRESS=../test/output/compress.exe
DECOMPRESS=../test/output/decompress.exe
compute $INPUT $COMPRESS $DECOMPRESS

printf "\nPress enter ..."
read 

INPUT=../test/input/big_input.txt
COMPRESS=../test/output/big_compress.txt
DECOMPRESS=../test/output/big_decompress.txt
compute $INPUT $COMPRESS $DECOMPRESS
