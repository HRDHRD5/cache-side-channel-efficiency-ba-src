#!/bin/bash

cd "$(dirname "$0")"

RESULT_FILE="result.csv"

echo "Building binary"
make

echo "Creating result File"
echo "bit_count;stride;training_length;input;output" > $RESULT_FILE

echo "Starting Measurements"

for ((i = 0; i < 100; i++)); do
    for ((stride = 256; stride <= 4096; stride *= 2)) do
        for ((bit_count = 4; bit_count <= 12; bit_count++)) do
            for ((train_length = 1; train_length <= 51; train_length += 10)) do
                ./measure $RANDOM $bit_count $stride $train_length >> $RESULT_FILE
            done
        done
    done
done