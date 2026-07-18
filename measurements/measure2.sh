#!/bin/bash

cd "$(dirname "$0")"

RESULT_FILE="result.csv"
TMP_FILE="tmp.csv"
COVERT_CHANNELS=( "frbm" )

MEASUREMENTS=100
stride=4096
train_length=20

echo "Saving CPU Specs"
lscpu > cpu-specs.txt
echo "" >> cpu-specs.txt
lscpu --caches >> cpu-specs.txt

echo "Building binary"
make

for COVERT_CHANNEL in "${COVERT_CHANNELS[@]}"
do
    echo "Starting Measurements for Channel: ${COVERT_CHANNEL}"
    echo ""
    echo "Creating result File"
    echo "n_b;n_a;stride;training_length;input;output;runtime" > $TMP_FILE

    echo "Starting Measurements"

    #echo "Run Nr ${i}/${MEASUREMENTS}"
    for ((n_b = 1; n_b <= 32; n_b++)) do
        for ((n_a = 1; n_a <= 1; n_a++)) do
            for ((i = 0; i < MEASUREMENTS; i++)); do
                ./measure "${COVERT_CHANNEL}" $i $n_b $n_a $stride $train_length >> $TMP_FILE
                #./throughput trnsf $n_b 1 $i >> $TMP_FILE
            done
        done
    done

    cat $TMP_FILE | grep -v "Training:" > "${COVERT_CHANNEL}-${RESULT_FILE}"
done