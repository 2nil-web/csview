#!/bin/bash

if [ -n "$1" ]; then
  nl=$1
else
  nl=100
fi

shuf -n$nl -i$(date -d '2020-01-01' '+%s')-$(date '+%s') | xargs -I{} date -d '@{}' '+%Y-%m-%dT%H:%M:%S%z' >rnd_date
shuf -n$nl -e TRACE DEBUG INFO WARN ERROR FATAL >rnd_ll
shuf -n$nl -i$(for i in {0..100}; do echo -n "TRACE DEBUG INFO WARN ERROR FATAL"; done) >rnd_ll
paste rnd_date rnd_ll
#rm -f rnd_date rnd_ll
exit


#for i in {0..100}; do echo -n "TRACE DEBUG INFO WARN ERROR FATAL"; done
