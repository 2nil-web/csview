#!/bin/bash

if [ -n "$1" ]; then
  nr=$1
else
  nr=20
fi

if [ -n "$2" ]; then
  nc=$2
else
  nc=10
fi

if [ -n "$3" ]; then
  cw=$3
else
  cw=40
fi

fname="mat-r${nr}-c${nc}-w${cw}.csv"
rm -f $fname
echo "Generating a file called $fname of $nr rows by $nc columns of $cw characters width for each column."
shuf -n${nr} -i$(date -d '2020-01-01' '+%s')-$(date '+%s') | xargs -I{} date -d '@{}' '+%Y-%m-%dT%H:%M:%S%z' >col0.csv

for ((c=1; c < ${nc}; c++)); do
  echo -n "col${c};" >>$fname
  cat /dev/urandom | tr -dc '[:alpha:]' | fold -w ${1:-${cw}} | head -n ${nr} >col$c.csv
  parm+="col${c}.csv "
done
echo "col${nc}" >>$fname
echo paste -d';' col0.csv $parm $fname
paste -d';' col0.csv $parm >>$fname
rm head.csv col0.csv $parm

