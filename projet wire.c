#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AVLNoeud {
    int stationID;
    long capacite;   // Capacité en kWh
    long consommation; // Consommation totale des consommateurs
    int taille;
    struct AVLNoeud *gauche;
    struct AVLNoeud *droite;
} AVLNoeud;

AVLNoeud* creerNoeud(int stationID, long capacite, long consommation) {
    AVLNoeud *noeud = (AVLNoeud*)malloc(sizeof(AVLNoeud));
    if (!noeud) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    noeud->stationID = stationID;
    noeud->capacite = capacite;
    noeud->consommation = consommation;
    noeud->taille = 1;
    noeud->gauche = noeud->droite = NULL;
    return noeud;
}

// Fonction pour obtenir la hauteur d'un noeud
int getTaille(AVLNoeud *noeud) {
    return noeud ? noeud->taille : 0;
}

// Calculer le facteur d'équilibre
int equilibre(AVLNoeud *noeud) {
    if (!noeud) return 0;
    return getTaille(noeud->gauche) - getTaille(noeud->droite);
}

// Rotation droite
AVLNoeud* rotationDroite(AVLNoeud *y) {
    AVLNoeud *x = y->gauche;
    AVLNoeud *T = x->droite;

    x->droite = y;
    y->gauche = T;

    y->taille = 1 + (getTaille(y->gauche) > getTaille(y->droite) ? getTaille(y->gauche) : getTaille(y->droite));
    x->taille = 1 + (getTaille(x->gauche) > getTaille(x->droite) ? getTaille(x->gauche) : getTaille(x->droite));

    return x;
}

// Rotation gauche
AVLNoeud* rotationGauche(AVLNoeud *x) {
    AVLNoeud *y = x->droite;
    AVLNoeud *T = y->gauche;

    y->gauche = x;
    x->droite = T;

    x->taille = 1 + (getTaille(x->gauche) > getTaille(x->droite) ? getTaille(x->gauche) : getTaille(x->droite));
    y->taille = 1 + (getTaille(y->gauche) > getTaille(y->droite) ? getTaille(y->gauche) : getTaille(y->droite));

    return y;
}

AVLNoeud* insererNoeud(AVLNoeud *noeud, int stationID, long capacite, long consommation) {
    if (!noeud) return creerNoeud(stationID, capacite, consommation);

    if (stationID < noeud->stationID)
        noeud->gauche = insererNoeud(noeud->gauche, stationID, capacite, consommation);
    else if (stationID > noeud->stationID)
        noeud->droite = insererNoeud(noeud->droite, stationID, capacite, consommation);
    else { // Si le nœud existe déjà, on met à jour sa consommation
        noeud->consommation += consommation;
        return noeud;
    }

    noeud->taille = 1 + (getTaille(noeud->gauche) > getTaille(noeud->droite) ? getTaille(noeud->gauche) : getTaille(noeud->droite));

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
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int stationID;
        long capacite = 0, consommation = 0;

        sscanf(line, "%d;%ld;%ld", &stationID, &capacite, &consommation);
        *racine = insererNoeud(*racine, stationID, capacite, consommation);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier CSV>\n", argv[0]);
        return 1;
    }

    AVLNoeud *racine = NULL;

    lireCSV(argv[1], &racine);

    printf("Contenu de l'arbre AVL :\n");
    parcoursInfixe(racine);
    freeAVL(racine);

    return 0;
}
