# ESAXX

This library provides the implementation of enhanced suffix array.  
For an input text of length N, this library builds an enhanced suffix array in O(N) time  
using 20N bytes.  

For a suffix array construction, I use sais.hxx, the induced sorting algorithm  
implemented by Yuta Mori.  

It also provides the program to enumerate the statistics of all substrings in the text.  

Daisuke Okanohara (daisuke dot okanohara at gmail.com)  

## Changes by this fork

- This program uses libdivsufsort to construct suffix array.  
- This program is built by CMake.  
- The default size of the type of interger is 64bits.  
- The input file must not contain the '0' character.
- I added the code outputting maximal substrings in an input file (enumMaximalSubstring.cpp). The code was copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it. Thank you.  

## Executions

enumMaximalSubstring.out  
This program computes all maximal substring of the input file.  
usage: ./enumMaximalSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -p, --print          print info (bool [=1])  
  -?, --help           print this message  

enumSubstring.out  
This program computes all internal nodes of the suffix tree of the input file.  
usage: ./enumSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -p, --print          print info (bool [=1])  
  -?, --help           print this message  

print.out
This program shows the above output files.  
usage: ./print.out --input_file=string --lcp_interval_file=string [options] ...  
options:  
  -i, --input_file           input file name (string)  
  -l, --lcp_interval_file    LCP interval file name (string)  
  -?, --help                 print this message  

## Example

echo -n GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT > sample.txt  
  
$ ./enumSubstring.out -i sample.txt -o sample.interval -p 1
>The internal nodes of the suffix tree of the file  
>  
>| id              | occurrence      | range(SA)       | string length   | string |  
>|:----------------|:----------------|:----------------|:----------------|:-------|
>| 0               | 4               | 2..5            | 2               | AC  |  
>| 1               | 3               | 6..8            | 3               | AGG  |  
>| 2               | 4               | 5..8            | 2               | AG  |  
>| 3               | 3               | 8..10           | 2               | AT  |  
>| 4               | 10              | 1..10           | 1               | A  |  
>| 5               | 4               | 10..13          | 2               | CA  |  
>| 6               | 7               | 10..16          | 1               | C  |  
>| 7               | 3               | 16..18          | 3               | GAC  |  
>| 8               | 3               | 18..20          | 4               | GAGG  |  
>| 9               | 6               | 16..21          | 2               | GA  |  
>| 10              | 3               | 22..24          | 4               | GGAC  |  
>| 11              | 3               | 25..27          | 3               | GGG  |  
>| 12              | 7               | 22..28          | 2               | GG  |  
>| 13              | 3               | 28..30          | 2               | GT  |  
>| 14              | 15              | 16..30          | 1               | G  |  
>| 15              | 4               | 32..35          | 2               | TG  |  
>| 16              | 7               | 30..36          | 1               | T  |  
>| 17              | 37              | 0..36           | 0  | |  
  
>----------RESULT----------  
>File: sample.txt  
>Output: sample.interval  
>The length of the input text: 36  
>The number of the internal nodes of the suffix tree of the input file: 18  
>----------RESULT----------  

$ ./enumMaximalSubstring.out -i sample.txt -o sample.ms -p 1
>Maximal substrings in the file
>  
>| id              | occurrence      | range(SA)       | string length   | string |  
>|:----------------|:----------------|:----------------|:----------------|:-------|
>|1                |4                |2..5             |2                |AC   |
>|2                |4                |5..8             |2                |AG   |
>|3                |3                |8..10            |2                |AT   |
>|4                |10               |1..10            |1                |A   |
>|5                |4                |10..13           |2                |CA   |
>|6                |7                |10..16           |1                |C   |
>|7                |3                |18..20           |4                |GAGG  | 
>|8                |6                |16..21           |2                |GA   |
>|9                |3                |22..24           |4                |GGAC   |
>|10               |3                |25..27           |3                |GGG   |
>|11               |7                |22..28           |2                |GG   |
>|12               |3                |28..30           |2                |GT   |
>|13               |15               |16..30           |1                |G   |
>|14               |4                |32..35           |2                |TG   |
>|15               |7                |30..36           |1                |T   |
>|16               |37               |0..36            |0   | |
>|17               |1                |20..20           |36               |GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(special end character)   |
  
>----------RESULT----------  
>File: sample.txt  
>Output: sample.ms  
>The length of the input text: 36  
>The number of maximum substrings: 17  
>----------RESULT----------  

$ ./print.out -i sample.txt -l sample.ms  
>| id              | occurrence      | range(SA)       | string length   | string |  
>|:----------------|:----------------|:----------------|:----------------|:-------|
>|1                |4                |2..5             |2                |AC   |
>|2                |4                |5..8             |2                |AG   |
>|3                |3                |8..10            |2                |AT   |
>|4                |10               |1..10            |1                |A   |
>|5                |4                |10..13           |2                |CA   |
>|6                |7                |10..16           |1                |C   |
>|7                |3                |18..20           |4                |GAGG  | 
>|8                |6                |16..21           |2                |GA   |
>|9                |3                |22..24           |4                |GGAC   |
>|10               |3                |25..27           |3                |GGG   |
>|11               |7                |22..28           |2                |GG   |
>|12               |3                |28..30           |2                |GT   |
>|13               |15               |16..30           |1                |G   |
>|14               |4                |32..35           |2                |TG   |
>|15               |7                |30..36           |1                |T   |
>|16               |37               |0..36            |0   | |
>|17               |1                |20..20           |36               |GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(special end character)   | 