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

## Download
The source codes in 'module' directory are maintained in different repositories. 
So, to download all the necessary source codes, do the following:

> git clone https://github.com/TNishimoto/esaxx.git  
> cd esaxx  
> git submodule init  
> git submodule update  

## Compile
> mkdir build  
> cd build  
> cmake -DCMAKE_BUILD_TYPE=Release ..  
> make  

## Executions && Examples

### enumMaximalSubstring.out  

This program computes all maximal substring of the input file.  
usage: ./enumMaximalSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -p, --print          print info (bool [=1])  
  -?, --help           print this message  

$ echo -n GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT > sample.txt  
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


### enumSubstring.out  

This program computes all internal nodes of the suffix tree of the input file.  
usage: ./enumSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -p, --print          print info (bool [=1])  
  -?, --help           print this message  

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

### print.out

This program shows the above output files.  
usage: ./print.out --input_file=string --lcp_interval_file=string [options] ...  
options:  
  -i, --input_file           input file name (string)  
  -l, --lcp_interval_file    LCP interval file name (string)  
  -?, --help                 print this message  

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

### enumKaiMaximalSubstring.out

This program receives positive texts and negative texts, and outputs each kai squared value of maximal substrings in the texts. 
The kai squared value of a maximal substring could be large when the most of occurrences of the maximal substring exists in positive (or negative) texts. 

usage: ./enumKaiMaximalSubstring.out --input_file=string [options] ... 
options:
  -i, --input_file          input file name (string)
  -o, --output_file         output file name (string [=])
  -p, --print               print info (bool [=1])
  -a, --occ_threshold       the threshold of occurrences (long long [=0])
  -b, --len_threshold       the threshold of length (long long [=0])
  -c, --occlen_threshold    the threshold of occ length (long long [=0])
  -?, --help                print this message

The input_file is the set of positive texts and negative texts.   
Each line in the file rerpesents a positive or negative text.  
The first character of each line is "+" or "-" and the remaining characters is the text of the line.  
The first character of the line is "+" if the text is positive instance; otherwise it is "-".  

The following text is an example of the input file for this program.  
>+GTAAGTATTTTTCAGCTTTTCATT  
>+TGTTTCAGCCTTAGTCATTATCG  
>+AGCTTTTCATTCTGACTGCAACGG  
>+CGTCCTGGATCTTTATTAGATCG  
>+AGCTTTTCATTCTGACTGCAACGG  
>-TCTCACTAAGATAAGCGACTGTCT  
>-CACACACGCACCCACACAGCCACA  
>-AATTGATTTTTCAGCAGCATTCG  
>-GTTGATAGAACACTAACCCTTCAG  
>-TAACATTTTGCGCCCTTTAAATAT

Caution 1 : Negative texts must follow positive texts in the file.  
Caution 2 : Positive and negative texts cannot contain the following characters: '+', '-', '(char)0', and '(char)1'  

$ ./enumKaiMaximalSubstring.out -i ../sample_texts/kai_text.txt

>|Kai_squared    |Bias   | MS(TRUE)     |  MS(FALSE)     | Others(TRUE)  |Others(FALSE) | Maximal_substring |  
>|:--------------|:------|:-------------|:---------------|:--------------|:-------------|:------------------|
>|9.704153       | -     | 0            |10              |423            |431           |CAC|  
>|7.745184       | -     | 0            |8               |423            |433           |ACA|  
>|5.795348       | -     | 0            |6               |423            |435           |CACA|  
>|5.258488       | -     | 4            |14              |419            |427           |AC|  
>|5.243108       | +     | 5            |0               |418            |441           |TCATT|  
>|4.823834       | -     | 0            |5               |423            |436           |ACAC|  
>|4.759463       | +     | 49           |32              |374            |409           |T|  

>___________RESULT___________  
>File: ../sample_texts/kai_text.txt  
>Output: ../sample_texts/kai_text.txt.0.0.0.csv  
>Occurrence threshold: 0  
>Length threshold: 0  
>Occurrence*Length threshold: 0  
>The length of the input text: 257  
>_________________________________