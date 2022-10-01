#!/bin/bash

# Create a csv file of nro rows by nco columns of nch characters wide for each columns being separated by the sep char
mkCsv() {
  [ $# -ge 1 ] && nro=$1 || nro=0
  [ $# -ge 2 ] && nco=$2 || nco=0
  [ $# -ge 3 ] && nch=$3 || nch=0
  [ $# -ge 4 ] && sep=$4 || sep=';'

  col=""
  for ((n=0; n < nch; n++)); do col="${col}$(expr $n % 10)"; done

  for ((r=0; r < nro; r++)); do
    for ((c=0; c < nco; c++)); do
      echo -n ${col}${sep}
    done
    echo
  done
}

mkLogCsv $1 $2

