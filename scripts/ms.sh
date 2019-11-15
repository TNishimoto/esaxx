#!/bin/sh
folderpath="/mnt/server/nishimoto/text"
outputpath="/mnt/server/nishimoto/output/rlbwt_txt"

filearr=( "fib41" "rs.13" "tm29" "dblp.xml.00001.1" "dblp.xml.00001.2" "dblp.xml.0001.1" "dblp.xml.0001.2" "sources.001.2" "dna.001.1" "proteins.001.1" "english.001.2" "einstein.de.txt" "einstein.en.txt" "world_leaders" "influenza" "kernel" "cere" "coreutils" "Escherichia_Coli" "para")

for file in ${filearr[@]}; do
	echo "esaxx ms ${file}"
    nohup /usr/bin/time -f "#esaxx ms ${file}, %e time, %M KB" ~/esaxx/build/enumMaximumSubstring.out -i ${folderpath}/${file} -o ${outputpath}/${file}.msi2 -p 0 >> /mnt/server/nishimoto/result/esaxx_ms_output.log
 done
 echo "Finished."