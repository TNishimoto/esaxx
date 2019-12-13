#!/bin/sh
folderpath="/Users/nishimoto/Documents/test_data/cantrbry"

filearr=( "alice29.txt" "cp.html" "grammar.lsp" "fields.c")

for file in ${filearr[@]}; do
	echo "esaxx lcp_interval_test ${file}"
    ./lcp_interval_test.out -i ${folderpath}/${file}
 done

for file in ${filearr[@]}; do
	echo "esaxx interval_test ${file}"
    ./interval_test.out -i ${folderpath}/${file}
 done

 echo "Finished."
