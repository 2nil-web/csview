#!/bin/bash

shuf -n1000 -i$(date -d '2020-01-01' '+%s')-$(date '+%s') | xargs -I{} date -d '@{}' '+%Y-%m-%dT%H:%M:%S%z' >rnd_date
shuf -n1000 -e TRACE DEBUG INFO WARN ERROR FATAL >rnd_ll
shuf -n1000 -i$(for i in {0..100}; do echo -n "TRACE DEBUG INFO WARN ERROR FATAL"; done) >rnd_ll
#paste rnd_date rnd_ll
#rm -f rnd_date rnd_ll


for i in {0..100}; do echo -n "TRACE DEBUG INFO WARN ERROR FATAL"; done
