#!/bin/bash

cd "$(dirname "$0")"

cd measurements/
./measure.sh
cd ../

cp measurements/result.csv evaluation/.

cd evaluation/
python3 evaluate.py