#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

#define BUFFER_SIZE 256

void readCSV(const char* filename, AVLNode** root, const char* stationType) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening CSV file");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER_SIZE];
    fgets(line, sizeof(line), file); // Skip the header

    while (fgets(line, sizeof(line), file)) {
        char hvbStationStr[16], hvaStationStr[16], lvStationStr[16];
        long capacity = 0, load = 0;

        sscanf(line, "%*[^;];%15[^;];%15[^;];%15[^;];%*[^;];%*[^;];%ld;%ld",
               hvbStationStr, hvaStationStr, lvStationStr, &capacity, &load);

        int stationID = -1;

        if (strcmp(stationType, "hva") == 0 && strcmp(hvaStationStr, "-") != 0) {
            stationID = atoi(hvaStationStr);
        } else if (strcmp(stationType, "hvb") == 0 && strcmp(hvbStationStr, "-") != 0) {
            stationID = atoi(hvbStationStr);
        } else if (strcmp(stationType, "lv") == 0 && strcmp(lvStationStr, "-") != 0) {
            stationID = atoi(lvStationStr);
        }

        if (stationID != -1 && (capacity > 0 || load > 0)) {
            *root = insertNode(*root, stationID, capacity, load);
            printf("Inserting: StationID=%d, Capacity=%ld, Consumption=%ld\n", stationID, capacity, load);
        }
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <station_type>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* stationType = argv[3];

    if (strcmp(stationType, "hvb") != 0 && strcmp(stationType, "hva") != 0 && strcmp(stationType, "lv") != 0) {
        fprintf(stderr, "Error: Invalid station type '%s'. Must be 'hvb', 'hva', or 'lv'.\n", stationType);
        return EXIT_FAILURE;
    }

    AVLNode* root = NULL;
    readCSV(inputFile, &root, stationType);

    FILE* output = fopen(outputFile, "w");
    if (!output) {
        perror("Error opening output file");
        freeTree(root);
        exit(EXIT_FAILURE);
    }

    fprintf(output, "StationID:Capacity:Consumption\n");
    inorderTraversal(root, output);

    fclose(output);
    freeTree(root);

    printf("Results saved to %s\n", outputFile);
    return EXIT_SUCCESS;
}
