#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

#define BUFFER_SIZE 256

void readCSV(const char* filename, AVLNode** root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening CSV file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip the header

    while (fgets(line, sizeof(line), file)) {
        char hvbStationStr[16], hvaStationStr[16], lvStationStr[16];
        long capacity = 0, load = 0;

        sscanf(line, "%*[^;];%15[^;];%15[^;];%15[^;];%*[^;];%*[^;];%ld;%ld",
               hvbStationStr, hvaStationStr, lvStationStr, &capacity, &load);

        // Convert station strings to integers or skip if "-"
        int hvaStation = (strcmp(hvaStationStr, "-") == 0) ? -1 : atoi(hvaStationStr);

        if (hvaStation != -1) { // Filter HV-A stations
            *root = insertNode(*root, hvaStation, capacity, load);
            printf("Inserting: StationID=%d, Capacity=%ld, Consumption=%ld\n", hvaStation, capacity, load);
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

    (void)stationType; // Supprime l'avertissement si la variable n'est pas utilisée

    AVLNode* root = NULL;
    readCSV(inputFile, &root);

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
