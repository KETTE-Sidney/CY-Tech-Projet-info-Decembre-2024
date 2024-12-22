#!/bin/bash

show_help() {
    echo "Usage: $0 <input_file> <output_file> <station_type>"
    echo
    echo "Arguments:"
    echo "  input_file    Path to the input CSV file"
    echo "  output_file   Path to the output CSV file"
    echo "  station_type  Type of station to process (hvb, hva, lv)"
    exit 0
}

if [[ "$1" == "-h" ]]; then
    show_help
fi

if [[ $# -ne 3 ]]; then
    echo "Error: Invalid number of arguments."
    show_help
fi

INPUT_FILE=$1
OUTPUT_FILE=$2
STATION_TYPE=$3

if [[ ! -f "$INPUT_FILE" ]]; then
    echo "Error: Input file '$INPUT_FILE' does not exist."
    exit 1
fi

if [[ "$STATION_TYPE" != "hvb" && "$STATION_TYPE" != "hva" && "$STATION_TYPE" != "lv" ]]; then
    echo "Error: Invalid station type '$STATION_TYPE'. Must be one of: hvb, hva, lv."
    exit 1
fi

mkdir -p tmp tests

cd codeC
if [[ ! -f projetwire ]]; then
    echo "Compiling the C program..."
    make
    if [[ $? -ne 0 ]]; then
        echo "Error: Compilation failed."
        exit 1
    fi
fi
cd ..

FILTERED_FILE="tmp/filtered_data.csv"
awk -F ";" -v station="$STATION_TYPE" 'BEGIN { OFS = ":" } {
    if (station == "hvb" && $2 != "-") {
        print $2, $7, $8
    } else if (station == "hva" && $3 != "-") {
        print $3, $7, $8
    } else if (station == "lv" && $4 != "-") {
        print $4, $7, $8
    }
}' "$INPUT_FILE" > "$FILTERED_FILE"

if [[ ! -s "$FILTERED_FILE" ]]; then
    echo "Error: No matching data found for the given station type."
    exit 1
fi

./codeC/projetwire "$FILTERED_FILE" "$OUTPUT_FILE" "$STATION_TYPE"
if [[ $? -ne 0 ]]; then
    echo "Error: Program execution failed."
    exit 1
fi

echo "Processing complete. Results saved to '$OUTPUT_FILE'."
