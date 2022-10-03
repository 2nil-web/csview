#!/bin/bash

rm -f test_res.txt

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

random_date () {
  shuf -n6 -i$(date -d '2020-01-01' '+%s')-$(date '+%s') | xargs -I{} date -d '@{}' '+%Y-%m-%dT%H:%M:%S%z'
}

mkLine () {
  col=""
  for ((n=0; n < $1; n++))
  do col="${col}$(expr $n % 10)"
  done
  echo $col
}

ts=$(mkLine 19)
ll=$(mkLine 5)
fn=$(mkLine 32)
ln=$(mkLine 5)

mkLogCsv () {
  [ $# -ge 1 ] && nro=$1 || nro=0
  [ $# -ge 2 ] && nch=$2 || nch=0
  [ $# -ge 3 ] && sep=$3 || sep=';'

  echo "TimeStamp;Log level;File;Line;Message"
  ms=$(mkLine $nch)
  row="${ts}${sep}${ll}${sep}${fn}${sep}${ln}${sep}${ms}${sep}"

  for ((r=0; r < nro; r++)); do
    echo $row
  done
}

nl=100000
mktestlog () {
  printf -v logname "test_${nl}x%03d.log" $1
  echo "Creating file $logname"
  mkLogCsv ${nl} $1 >>$logname
}

if false; then
  mktestlog   0
  mktestlog   8
  mktestlog  16
  mktestlog  32
  mktestlog  64
  mktestlog 128
fi
  mktestlog 256

for i in test_${nl}x???.log; do
	./mywc "$i" > test_res.txt
	/usr/bin/time -pao test_res.txt ./mytail 0 "$i"
done


cat test_res.txt

