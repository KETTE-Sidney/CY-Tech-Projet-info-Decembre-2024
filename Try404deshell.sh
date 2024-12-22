#!/bin/bash

# Function to display help
show_help() {
    echo "Usage: c-wire.sh <csv_file> <station_type> <consumer_type> [central_id] [-h]"
    echo "Parameters:"
    echo "  csv_file: Path to the input CSV file."
    echo "  station_type: hvb | hva | lv"
    echo "  consumer_type: comp"
    echo "  central_id: (optional) Filter by a specific central ID."
    echo "  -h: Display this help message."
    exit 0
}

# Parse arguments
if [[ "$1" == "-h" || $# -lt 3 ]]; then
    show_help
fi

CSV_FILE=$1
STATION_TYPE=$2
CONSUMER_TYPE=$3
CENTRAL_ID=$4

# Validate inputs
if [[ ! -f "$CSV_FILE" ]]; then
    echo "Error: CSV file not found."
    show_help
fi
if [[ "$STATION_TYPE" != "hvb" && "$STATION_TYPE" != "hva" && "$STATION_TYPE" != "lv" ]]; then
    echo "Error: Invalid station type. Allowed values: hvb, hva, lv."
    show_help
fi
if [[ "$CONSUMER_TYPE" != "comp" ]]; then
    echo "Error: Invalid consumer type. Only 'comp' is allowed."
    show_help
fi

# Set up directories
mkdir -p tmp graphs
if [[ $? -ne 0 ]]; then
    echo "Error: Unable to create required directories."
    exit 1
fi
rm -rf tmp/*

# Check for compiled C program
if [[ ! -f "codeC/main" ]]; then
    echo "Compiling C program..."
    (cd codeC && make)
    if [[ $? -ne 0 ]]; then
        echo "Error: Compilation failed. Please check your C code."
        exit 1
    fi
fi

# Filter data based on station type and optional central ID
echo "Filtering data..."
if [[ -n "$CENTRAL_ID" ]]; then
    awk -F';' -v station="$STATION_TYPE" -v central="$CENTRAL_ID" '
    BEGIN { OFS=";" }
    {
        if ((station == "hvb" && $2 != "-" && $3 == "-" && $4 == "-") ||
            (station == "hva" && $3 != "-" && $4 == "-") ||
            (station == "lv" && $4 != "-")) {
            if ($1 == central) print $0
        }
    }' "$CSV_FILE" > tmp/filtered_data.csv
else
    awk -F';' -v station="$STATION_TYPE" '
    BEGIN { OFS=";" }
    {
        if ((station == "hvb" && $2 != "-" && $3 == "-" && $4 == "-") ||
            (station == "hva" && $3 != "-" && $4 == "-") ||
            (station == "lv" && $4 != "-")) {
            print $0
        }
    }' "$CSV_FILE" > tmp/filtered_data.csv
fi

# Run C program
echo "Running C program..."
codeC/main tmp/filtered_data.csv > tmp/results.csv
if [[ $? -ne 0 ]]; then
    echo "Error: C program encountered an error."
    exit 1
fi

# Print completion message
echo "Processing completed. Results saved in tmp/results.csv."


