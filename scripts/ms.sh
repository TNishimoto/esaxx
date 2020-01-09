#!/bin/sh
folderpath="/mnt/server/nishimoto/text"
outputpath="/mnt/server/nishimoto/output/rlbwt_txt"

filearr=( "fib41" "rs.13" "tm29" "dblp.xml.00001.1" "dblp.xml.00001.2" "dblp.xml.0001.1" "dblp.xml.0001.2" "sources.001.2" "dna.001.1" "proteins.001.1" "english.001.2" "einstein.de.txt" "einstein.en.txt" "world_leaders" "influenza" "kernel" "cere" "coreutils" "Escherichia_Coli" "para")

for file in ${filearr[@]}; do
	echo "online rlbwt ${file}"
    nohup /usr/bin/time -f "#online rlbwt ${file}, %e sec, %M KB" ~/esaxx/build/online_rlbwt.out -i ${folderpath}/${file} -o ${outputpath}/${file}.rlbwt >> /mnt/server/nishimoto/result/esaxx_online_rlbwt_output_new.log
 done


for file in ${filearr[@]}; do
	echo "esaxx ms(esaxx) ${file}"
    nohup /usr/bin/time -f "#esaxx ms(esaxx) ${file}, %e sec, %M KB" ~/esaxx/build/enumMaximalSubstring.out -i ${folderpath}/${file} -o ${outputpath}/${file}.msi2 -p 0 -m esaxx >> /mnt/server/nishimoto/result/esaxx_ms_output_new.log
	echo "esaxx ms(rlbwt) ${file}"
    nohup /usr/bin/time -f "#esaxx ms(rlbwt) ${file}, %e sec, %M KB" ~/esaxx/build/enumMaximalSubstring.out -i ${outputpath}/${file}.rlbwt -o ${outputpath}/${file}.rlbwt.msi2 -p 0 -m rlbwt >> /mnt/server/nishimoto/result/esaxx_ms_rlbwt_output_new.log
 done
 echo "Finished."

filearr2=( "fib41" "rs.13" "dblp.xml.00001.1" "dblp.xml.00001.2" "dblp.xml.0001.1" "dblp.xml.0001.2" "sources.001.2" "dna.001.1" "proteins.001.1" "english.001.2" "einstein.de.txt" "einstein.en.txt" "world_leaders" "influenza" "kernel" "cere" "coreutils" "Escherichia_Coli" "para")

#for file in ${filearr2[@]}; do
#	echo "esaxx ms(sdsl) ${file}"
#    nohup /usr/bin/time -f "#esaxx ms(sdsl) ${file}, %e sec, %M KB" ~/esaxx/build/enumMaximalSubstring.out -i ${folderpath}/${file} -o ${outputpath}/${file}.sdsl.msi2 -p 0 -m sdsl -u 1 >> /mnt/server/nishimoto/result/esaxx_ms_sdsl_output.log
# done
# echo "Finished."
 