#!/bin/bash

cd "$(dirname "$0")"

RESULT_FILE="result.csv"
PROPS_FILE="prop-measure.csv"
TMP_FILE="tmp.csv"
COVERT_CHANNELS=( "frm" )

MEASUREMENTS=100
stride=4096
train_length=20
nb_max=10
na_props=4

echo "Saving CPU Specs"
lscpu > cpu-specs.txt
echo "" >> cpu-specs.txt
lscpu --caches >> cpu-specs.txt

echo "" > $TMP_FILE

echo "Building binary"
make

echo "Starting Properties Measurements"
echo ""
echo "Creating result File"
echo "n_b;n_a;stride;training_length;input;output;runtime" > "result-${PROPS_FILE}"
echo "stat;N_P;T_P;N_T;T_T;L_E;N_R;T_R" > "props-${PROPS_FILE}"

echo "Starting Properties Measurements"

for ((i = 0; i < MEASUREMENTS; i++)); do
    ./measure "mprop" $i $nb_max $na_props $stride $train_length >> $TMP_FILE
done

cat $TMP_FILE | grep -vE "^$" | grep "props;" >> "props-${PROPS_FILE}"
cat $TMP_FILE | grep -vE "^$" | grep -v "Training:" | grep -v "props;" >> "result-${PROPS_FILE}"

echo "" > $TMP_FILE

for COVERT_CHANNEL in "${COVERT_CHANNELS[@]}"
do
    echo "Starting Measurements for Channel: ${COVERT_CHANNEL}"
    echo ""
    echo "Creating result File"
    echo "n_b;n_a;stride;training_length;input;output;runtime" > $TMP_FILE

    echo "Starting Measurements"

    #echo "Run Nr ${i}/${MEASUREMENTS}"
    for ((n_b = 10; n_b <= 10; n_b++)) do
        for ((n_a = 1; n_a <= 6; n_a++)) do
        #for ((n_a = 1; n_a <= 1; n_a++)) do
            for ((i = 0; i < MEASUREMENTS; i++)); do
                ./measure "${COVERT_CHANNEL}" $i $n_b $n_a $stride $train_length >> $TMP_FILE
            done
        done
    done

    cat $TMP_FILE | grep -v "Training:" > "${COVERT_CHANNEL}1-${RESULT_FILE}"
done

echo "" > $TMP_FILE

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
            done
        done
    done

    cat $TMP_FILE | grep -v "Training:" > "${COVERT_CHANNEL}2-${RESULT_FILE}"
done