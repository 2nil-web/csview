#!/bin/bash

let ts_min=$(date -d '2020-01-01' '+%s')
let ts_max=$(date '+%s')
let delta=$ts_max-$ts_min
echo "ts min=$ts_min, ts max=$ts_max, delta=$delta"

for i in {1..10}; do
  let rndelta=$RANDOM % $delta
  let ts=$ts_min + 1 + $rndelta
  echo "rndelta=$rndelta, ts=$ts"
  dt=$(date -d "@$ts" '+%Y-%m-%dT%H:%M:%S%z')
  echo "$ts == $dt"
done

