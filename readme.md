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

You need sdsl-lite(https://github.com/simongog/sdsl-lite) to excecute this program. Please edit CMakeLists.txt to set SDSL library and include directory paths.

## Compile
> mkdir build  
> cd build  
> cmake -DCMAKE_BUILD_TYPE=Release -DCOMPILE_LIBDIVSUFSORT=TRUE ..  
> make  

## Executions && Examples

### enumSubstring.out  

This program computes all internal nodes of the suffix tree of the input file. 
usage: ./enumMaximalSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -?, --help           print this message  

$ echo -n GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT > sample.txt  
$ ./enumSubstring.out -i sample.txt -o sample.interval

>----------RESULT----------  
>File: sample.txt  
>Output: sample.interval  
>The length of the input text: 36  
>The number of the internal nodes of the suffix tree of the input file: 18  
>----------RESULT----------

$ ./print.out -i sample.txt -l sample.interval

>Loading: sample.interval  
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

>GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$ Text  
>--------------ACAC AG        AC     (AG)  
>-ATC    AGG          AGG            (ATC)  
>-AT     AG         AGAG             (AT)  
>-ATCAAT                             (CA)  
>-A CAA  A     A A  A A       A      (C)  
>---CA          CACCA                (CC)  
>---C         G C CC     C     C     (G)  
>-------------GAC    GAG     GAC     (GAG)  
>GATC   GAGG         GAGG            (GATC)  
>GA     GA    GA     GA GC   GA      (GC)  
>------------GGAC      GGCG GGAC     (GGCG)  
>---------GGT             GGGG       (GGT)  
>---------GG GG        GG GGGG    GT (GT)  
>----------GT                     GT$(T$)  
>G      G GG GG      G GG GGGG    GT (T)  
>------TG   TG                  TTG  (TT)  
>--T   T    T          G        TT T (G)  

### enumMaximalSubstring.out  

This program computes all maximal substrings of the input file using ESAXX ilbrary (https://github.com/hillbig/esaxx).  
usage: ./enumSubstring.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string [=])  
  -?, --help           print this message  

$ ./enumMaximalSubstring.out -i sample.txt -o sample.ms

>----------RESULT----------  
>File: sample.txt  
>Output: sample.ms  
>The length of the input text: 36  
>The number of maximum substrings: 17  
>----------RESULT----------  

$ ./print.out -i sample.txt -l sample.ms

>Maximal substrings in the file
> 
>|id      |occurrence      |range(SA)       |string length   |string |
>|:-------|:---------------|:---------------|:---------------|-------|
>|0       |1               |20..20          |36              |GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(special end character)   |
>|1       |4               |2..5            |2               |AC   |
>|2       |4               |5..8            |2               |AG   |
>|3       |3               |8..10           |2               |AT   |
>|4       |10              |1..10           |1               |A   |
>|5       |4               |10..13          |2               |CA   |
>|6       |7               |10..16          |1               |C   |
>|7       |3               |18..20          |4               |GAGG   |
>|8       |6               |16..21          |2               |GA   |
>|9       |3               |22..24          |4               |GGAC   |
>|10      |3               |25..27          |3               |GGG   |
>|11      |7               |22..28          |2               |GG   |
>|12      |3               |28..30          |2               |GT   |
>|13      |15              |16..30          |1               |G   |
>|14      |4               |32..35          |2               |TG   |
>|15      |7               |30..36          |1               |T   |
>|16      |37              |0..36           |0               |   |
  
> GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$ Text  
> --------------ACAC           AC     (AC)  
> --------AG         AGAG             (AG)  
> -AT  AT                             (AT)  
> -A  AA  A     A A  A A       A      (A)  
> ---CA          CA CA                (CA)  
> ---C           C CC     C     C     (C)  
> -------GAGG         GAGG            (GAGG)  
> GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$)  
> GA     GA    GA     GA      GA      (GA)  
> ------------GGAC           GGAC     (GGAC)  
> -------------------------GGGG       (GGG)  
> ---------GG GG        GG GGGG       (GG)  
> ----------GT                     GT (GT)  
> G      G GG GG      G GG GGGG    G  (G)  
> ------TG   TG                   TG  (TG)  
> --T   T    T                   TT T (T)  


### online_rlbwt.out  

This program outputs the Run-length BWT of an input file using the library (https://github.com/itomomoti/OnlineRlbwt). 

usage: ./online_rlbwt.out --input_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (the default output name is 'input_file.ext') (string [=])  
  -t, --input_type     input_type (string [=text])  
  -y, --output_type    output_type (string [=rlbwt])  
  -?, --help           print this message  

$ ./online_rlbwt.out -i sample.txt -o sample.rlbwt

>-------------RESULT--------------  
>File : sample.txt    
>FileType : text  
>Output : sample.rlbwt  
>OutputType : rlbwt  
>The length of the input text : 36  
>The number of runs : 31  
>Excecution time : 5ms[7.2chars/ms]  
>----------------------------------  

### enumMaximalSubstringWithRLBWT.out

This program computes all maximal substrings of the string recovered from the input RLBWT in compressed space.  

usage: ./enumMaximalSubstringWithRLBWT.out --input_file=string [options] ...   
options:  
  -i, --input_file     input RLBWT file name (string)  
  -o, --output_file    output file name (string [=])  
  -?, --help           print this message  

$ ./enumMaximalSubstringWithRLBWT.out -i sample.rlbwt -o sample.ms2

> ______________________RESULT______________________  
> RLBWT File                                       : sample.rlbwt  
> Output                                   : sample.ms2  
> The length of the input text             : 36  
> The number of maximum substrings         : 17  
> Excecution time                          : 1[ms]  
> |        RLBWT loading time              : 0[ms]  
> |        Sampling SA construction time   : 0[ms]  
> |        Sampling LCP construction time  : 1[ms]  
> |        Sampling LCP & SA construction time     : 1[ms]  
> |        MS Construction time                    : 0[ms]  
> _______________________________________________________  


$ ./print.out -i sample.txt -l sample.ms2

>Maximal substrings in the file
> 
>|id      |occurrence      |range(SA)       |string length   |string |
>|:-------|:---------------|:---------------|:---------------|-------|
>|0       |1               |20..20          |36              |GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(special end character)   |
>|1       |4               |2..5            |2               |AC   |
>|2       |4               |5..8            |2               |AG   |
>|3       |3               |8..10           |2               |AT   |
>|4       |10              |1..10           |1               |A   |
>|5       |4               |10..13          |2               |CA   |
>|6       |7               |10..16          |1               |C   |
>|7       |3               |18..20          |4               |GAGG   |
>|8       |6               |16..21          |2               |GA   |
>|9       |3               |22..24          |4               |GGAC   |
>|10      |3               |25..27          |3               |GGG   |
>|11      |7               |22..28          |2               |GG   |
>|12      |3               |28..30          |2               |GT   |
>|13      |15              |16..30          |1               |G   |
>|14      |4               |32..35          |2               |TG   |
>|15      |7               |30..36          |1               |T   |
>|16      |37              |0..36           |0               |   |
  
> GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$ Text  
> --------------ACAC           AC     (AC)  
> --------AG         AGAG             (AG)  
> -AT  AT                             (AT)  
> -A  AA  A     A A  A A       A      (A)  
> ---CA          CA CA                (CA)  
> ---C           C CC     C     C     (C)  
> -------GAGG         GAGG            (GAGG)  
> GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$(GATCAATGAGGTGGACACCAGAGGCGGGGACTTGT$)  
> GA     GA    GA     GA      GA      (GA)  
> ------------GGAC           GGAC     (GGAC)  
> -------------------------GGGG       (GGG)  
> ---------GG GG        GG GGGG       (GG)  
> ----------GT                     GT (GT)  
> G      G GG GG      G GG GGGG    G  (G)  
> ------TG   TG                   TG  (TG)  
> --T   T    T                   TT T (T)  