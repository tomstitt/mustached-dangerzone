#! /bin/bash

POWERS_OF_TWO=(1 2 4 8 16 32 64 128)
INPUT_FILES=(1_input_100kb.txt 2_input_1MB.txt 3_input_10MB.txt)
#POWERS_OF_TWO=(1 )
#INPUT_FILES=(1_input_100kb.txt )
EXEC=cmpsc473mr

echo -e "filename, thread-count, buffer-size, runtime"
for file in "${INPUT_FILES[@]}"; do
    for thds in "${POWERS_OF_TWO[@]}"; do
        for buffer_size in "${POWERS_OF_TWO[@]}"; do
            t=$(time (./$EXEC $file $thds $buffer_size > /dev/null 2>&1) 2>&1)
            t=$(echo $t | sed "s/ /\n/g" | egrep -o "[0-9]+\.[0-9]+" | head -n 1)
            echo -e "${file}, ${thds}, ${buffer_size}, ${t}"
        done
    done
done
