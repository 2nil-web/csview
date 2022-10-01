#!/bin/bash

for i in file_10*.csv; do
  echo
  ./mywc.exe "$i"; time ./mytail.exe 0 "$i" | tee -a toto.txt | grep -E '^real|^user|^sys'
done

