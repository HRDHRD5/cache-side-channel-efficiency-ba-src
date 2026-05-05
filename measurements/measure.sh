#!/bin/bash

cd "$(dirname "$0")"

RESULT_FILE="result.csv"
TMP_FILE="tmp.csv"
COVERT_CHANNELS=( "frb" "fra" )

MEASUREMENTS=250

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
    echo "bit_count;stride;training_length;input;output;runtime" > $TMP_FILE

    echo "Starting Measurements"

    for ((i = 0; i < MEASUREMENTS; i++)); do
        echo "Run Nr ${i}/${MEASUREMENTS}"
        for ((stride = 64; stride <= 4096; stride *= 2)) do
            for ((bit_count = 1; bit_count <= 10; bit_count++)) do
                for ((train_length = 0; train_length <= 150; train_length += 50)) do
                    ./measure "${COVERT_CHANNEL}" $RANDOM $bit_count $stride $train_length >> $TMP_FILE
                done
            done
        done
    done

    cat $TMP_FILE | grep -v "Training:" > "${COVERT_CHANNEL}-${RESULT_FILE}"
done