#!/bin/sh
folderpath="/mnt/server/nishimoto/text"
outputpath="/mnt/server/nishimoto/output/rlbwt_txt"

filearr=( "fib41" "rs.13" "tm29" "dblp.xml.00001.1" "dblp.xml.00001.2" "dblp.xml.0001.1" "dblp.xml.0001.2" "sources.001.2" "dna.001.1" "proteins.001.1" "english.001.2" "einstein.de.txt" "einstein.en.txt" "world_leaders" "influenza" "kernel" "cere" "coreutils" "Escherichia_Coli" "para")

for file in ${filearr[@]}; do
	echo "Construction of RLESA Standard algorithm: ${file}"
    nohup /usr/bin/time -f "#Construction of RLESA Standard algorithm, ${file}, %e sec, %M KB" ~/esaxx/build/rlesa.out -i ${folderpath}/${file} -m 0 >> /mnt/server/nishimoto/result/esaxx_rlesa.log
 done
for file in ${filearr[@]}; do
	echo "Construction of RLESA Faster algorithm: ${file}"
    nohup /usr/bin/time -f "#Construction of RLESA Faster algorithm, ${file}, %e sec, %M KB" ~/esaxx/build/rlesa.out -i ${folderpath}/${file} -m 1 >> /mnt/server/nishimoto/result/esaxx_rlesa.log
 done


 echo "Finished."
