#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AVLNoeud {
    int stationID;
    long capacite;   // Capacité en kWh
    long consommation; // Consommation totale des consommateurs
    int hauteur;
    struct AVLNoeud *gauche;
    struct AVLNoeud *droite;
} AVLNoeud;

AVLNoeud* creerNoeud(int stationID, long capacite, long consommation) {
    AVLNoeud *noeud = (AVLNoeud*)malloc(sizeof(AVLNoeud));
    if (!noeud) {
        perror("Erreur d'allocation mémoire");
        exit(1);
    }
    noeud->stationID = stationID;
    noeud->capacite = capacite;
    noeud->consommation = consommation;
    noeud->hauteur = 1;
    noeud->gauche = noeud->droite = NULL;
    return noeud;
}

// Fonction pour obtenir la hauteur d'un noeud
int gethauteur(AVLNoeud *noeud) {
    return noeud ? noeud->hauteur : 0;
}

// Calculer le facteur d'équilibre
int equilibre(AVLNoeud *noeud) {
    if (!noeud) return 0;
    return gethauteur(noeud->gauche) - gethauteur(noeud->droite);
}
// possible en une fois avec  return noeud ? hauteur(noeud->gauche) - hauteur(noeud->droite) : 0;


// Rotation droite
AVLNoeud* rotationDroite(AVLNoeud *y) {
    AVLNoeud *x = y->gauche;
    AVLNoeud *T = x->droite;

    x->droite = y;
    y->gauche = T;

    y->taille = 1 + (gethauteur(y->gauche) > gethauteur(y->droite) ? gethauteur(y->gauche) : gethauteur(y->droite));
    x->taille = 1 + (gethauteur(x->gauche) > gethauteur(x->droite) ? gethauteur(x->gauche) : gethauteur(x->droite));

    return x;
}

// Rotation gauche
AVLNoeud* rotationGauche(AVLNoeud *x) {
    AVLNoeud *y = x->droite;
    AVLNoeud *T = y->gauche;

    y->gauche = x;
    x->droite = T;

    x->taille = 1 + (gethauteur(x->gauche) > gethauteur(x->droite) ? gethauteur(x->gauche) : gethauteur(x->droite));
    y->taille = 1 + (gethauteur(y->gauche) > gethauteur(y->droite) ? gethauteur(y->gauche) : gethauteur(y->droite));

    return y;
}

AVLNoeud* insererNoeud(AVLNoeud *noeud, int stationID, long capacite, long consommation) {
    if (!noeud) { return creerNoeud(stationID, capacite, consommation); }

    if (stationID < noeud->stationID)
        noeud->gauche = insererNoeud(noeud->gauche, stationID, capacite, consommation);
    else if (stationID > noeud->stationID)
        noeud->droite = insererNoeud(noeud->droite, stationID, capacite, consommation);
    else { // Si le nœud existe déjà, on met à jour sa consommation
        noeud->consommation += consommation;
        return noeud;
    }

    noeud->hauteur = 1 + (gethauteur(noeud->gauche) > gethauteur(noeud->droite) ? gethauteur(noeud->gauche) : gethauteur(noeud->droite));

    int h = equilibre(noeud);

    // Rotation pour équilibrer
    if (h > 1 && stationID < noeud->gauche->stationID)
        return rotationDroite(noeud);

    if (h < -1 && stationID > noeud->droite->stationID)
        return rotationGauche(noeud);

    if (h > 1 && stationID > noeud->gauche->stationID) {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    if (h < -1 && stationID < noeud->droite->stationID) {
        noeud->droite = rotationDroite(noeud->droite);
        return rotationGauche(noeud);
    }

    return noeud;
}

void parcoursInfixe(AVLNoeud *racine) {
    if (racine) {
        parcoursInfixe(racine->gauche);
        printf("Station ID: %d, Capacite: %ld, Consommation: %ld\n", racine->stationID, racine->capacite, racine->consommation);
        parcoursInfixe(racine->droite);
    }
}

void freeAVL(AVLNoeud *racine) {
    if (racine) {
        freeAVL(racine->gauche);
        freeAVL(racine->droite);
        free(racine);
    }
}

// Traitement du fichier CSV
void lireCSV(const char *filename, AVLNoeud **racine) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        exit(1);
    }

   char line[256];
    while (fgets(line, sizeof(line), file)) {
        int stationID;
        long capacite = 0, consommation = 0;
        char type_station[10], type_consommateur[10];

        // Lecture des colonnes du fichier CSV (modifiez selon votre format exact)
        sscanf(line, "%d;%ld;%ld;%[^;];%[^;]", &stationID, &capacite, &consommation, type_station, type_consommateur);

        // Appliquer les filtres en fonction des arguments
        if ((strcmp(station_type, "all") == 0 || strcmp(station_type, type_station) == 0) &&
            (strcmp(consumer_type, "all") == 0 || strcmp(consumer_type, type_consommateur) == 0) &&
            (plant_id == -1 || plant_id == stationID)) {
            *racine = insererNoeud(*racine, stationID, capacite, consommation);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fichier CSV> <output_file>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    const char *station_type = argv[3];
    
    AVLNoeud *racine = NULL;

    lireCSV(argv[1], &racine);

FILE* output = fopen(output_file, "w");
    if (!output) {
        perror("Erreur lors de l'ouverture du fichier de sortie.");
        fclose(input);
        return 1;
    }

    printf("Contenu de l'arbre AVL :\n");
    parcoursInfixe(racine);
    freeAVL(racine);

    return 0;
}

/*int main(int argc, char* argv[]) {
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
}*/ 
