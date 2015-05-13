#! /bin/bash

REPLICA_SET=(1 5 10 50 100)
BUFFER_SET=(1 5 10 50 100)
INPUT_FILES=(1_input_100kb.txt 2_input_1MB.txt 3_input_10MB.txt)
#POWERS_OF_TWO=(1 )
#INPUT_FILES=(1_input_100kb.txt )
EXEC=project
NUM_TRIALS=20
OUTFILE_BASE=timing_out

echo -e "filename, thread-count, buffer-size, runtime"
for trial in $(seq 1 $NUM_TRIALS); do
    for file in "${INPUT_FILES[@]}"; do
        for thds in "${REPLICA_SET[@]}"; do
            for buffer_size in "${BUFFER_SET[@]}"; do
                t=$(time (./$EXEC $file $thds $buffer_size > /dev/null 2>&1) 2>&1)
                t=$(echo $t | sed "s/ /\n/g" | egrep -o "[0-9]+\.[0-9]+" | head -n 1)
                echo -e "${file}, ${thds}, ${buffer_size}, ${t}" | tee -a ${OUTFILE_BASE}_${trial}.csv
            done
        done
    done
done
