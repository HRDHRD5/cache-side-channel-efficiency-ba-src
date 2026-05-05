#!/bin/bash

cd "$(dirname "$0")"

cd measurements/
./measure.sh
cd ../

cp measurements/result.csv evaluation/.
cp measurements/cpu-specs.txt evaluation/.

cd evaluation/
python3 evaluate.py