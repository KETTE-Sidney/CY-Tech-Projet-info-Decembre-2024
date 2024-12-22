#include "avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction utilitaire pour convertir un champ en entier ou 0 s'il est vide
long parse_long(const char* str) {
    char *endptr;

    if (strcmp(str, "-") == 0) {
        return 0; // Remplace "-" par 0
    }

    long number = strtol(str, &endptr, 10);

    // Check for conversion errors
    if (*endptr != '\0') {
        printf("Conversion error, non-numeric characters found: %s\n", endptr);
    }

    return number;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    const char *station_type = argv[3]; // Récupération du STATION_TYPE

    FILE* input = fopen(input_file, "r");
    if (!input) {
        perror("Erreur lors de l'ouverture du fichier d'entrée.");
        return 1;
    }

    FILE* output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur lors de l'ouverture du fichier de sortie.");
        fclose(input);
        return 1;
    }

    AVLNode* root = NULL;
    char line[256];

    // Lire les données et insérer dans l'arbre AVL
    while (fgets(line, sizeof(line), input)) {
        // Suppression des sauts de ligne
        line[strcspn(line, "\n")] = 0;

        if ((strncmp(line, "LV Station:Capacity:Load", 24) == 0 && strcmp(station_type, "lv") == 0) ||
            (strncmp(line, "HV-A Station:Capacity:Load", 25) == 0 && strcmp(station_type, "hva") == 0) ||
            (strncmp(line, "HV-B Station:Capacity:Load", 25) == 0 && strcmp(station_type, "hvb") == 0)) {
            printf("DEBUG : Ligne d'en-tête ignorée : %s\n", line);
            continue;
        }


        char* token = strtok(line, ":");
        if (!token) {
            fprintf(stderr, "DEBUG : Ligne ignorée (station_id manquant) : %s\n", line);
            continue;
        }
        long station_id = parse_long(token);

        token = strtok(NULL, ":");
        if (!token) {
            fprintf(stderr, "DEBUG : Ligne ignorée (station_id manquant) : %s\n", line);
            continue;
        }
        long capacity = parse_long(token);

        token = strtok(NULL, ":");
        if (!token) {
            fprintf(stderr, "DEBUG : Ligne ignorée (station_id manquant) : %s\n", line);
            continue;
        }
        long load = parse_long(token);

        
        root = insert(root, station_id, capacity, load);
    }

    // Écrire les résultats triés par station ID
    fprintf(output, "StationID:Capacity:Consumption\n");
    inorder_traversal(root, output);

    // Libérer la mémoire et fermer les fichiers
    free_tree(root);
    fclose(input);
    fclose(output);

    return 0;
}