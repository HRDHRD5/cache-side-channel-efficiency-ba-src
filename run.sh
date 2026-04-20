#!/bin/bash

cd "$(dirname "$0")"

cd measurements/
./measure.sh
cd ../

cp measurements/bits_and_stride.csv evaluation/.
cp measurements/training_length.csv evaluation/.

cd evaluation/
python3 evaluate.py