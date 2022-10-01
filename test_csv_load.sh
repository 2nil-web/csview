#!/bin/bash

./mkcsv.sh 10000 10   0 >file_10000x10x000.csv
./mkcsv.sh 10000 10   8 >file_10000x10x008.csv
./mkcsv.sh 10000 10  16 >file_10000x10x016.csv
./mkcsv.sh 10000 10  32 >file_10000x10x032.csv
./mkcsv.sh 10000 10  64 >file_10000x10x064.csv
./mkcsv.sh 10000 10 128 >file_10000x10x128.csv
./mkcsv.sh 10000 10 255 >file_10000x10x255.csv

for i in file_10000*.csv; do
	( ./mywc "$i"; time ./mytail 0 "$i" ) >>res.txt
done

grep -E '^file_10000|^real|^user|^sys' res.txt
#rm res.txt

