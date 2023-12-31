#!/bin/bash

# Check for command line parameter
if [ "$#" -ne 1 ]; then
    echo "Please provide the IOs directory path as a command line parameter."
    exit 1
fi

# Assign the IOs directory to a variable
IOsDir="$1"

# Compile the program
g++ -std=c++14 -O3 main.cpp -o main.exe vector.cpp matrix.cpp transform.cpp triangle.cpp line.cpp

# Check if compilation was successful
if [ ! -f main.exe ]; then
    echo "Compilation failed."
    exit 1
fi

# Iterate over each subfolder in the IOs directory
for i in "$IOsDir"/*; do
    if [ -d "$i" ]; then
        echo "Processing folder: $i"

        # Delete existing output files from the script's directory
        rm -f "stage1.txt" "stage2.txt" "stage3.txt" "z_buffer.txt" "out.bmp"

        # Run the program with the scene and config files as input
        ./main.exe "$i/scene.txt" "$i/config.txt"

        # Initialize a variable to track success
        matched=true

        # Compare each output file with the expected output
        for f in stage1.txt stage2.txt stage3.txt z_buffer.txt; do
            if [ -f "$i/$f" ]; then
                diff -w "$f" "$i/$f" >/dev/null || {
                    echo "Difference found in $f:"
                    diff -w "$f" "$i/$f"
                    matched=false
                }
            else
                echo "Expected output file $f is missing in folder $i."
                matched=false
            fi
        done

        # Perform a binary comparison for the .bmp file
        if [ -f "out.bmp" ]; then
            cmp -s "out.bmp" "$i/out.bmp" || {
                echo "Difference found in out.bmp:"
                cmp "out.bmp" "$i/out.bmp"
                matched=false
            }
        else
            echo "Expected output file out.bmp is missing in folder $i."
            matched=false
        fi

        # Output the result
        if $matched; then
            echo "Success: All files match."
        else
            echo "Fail: There were differences."
        fi

        echo "--------------------------------"
    fi
done

echo "Finished processing all folders."
