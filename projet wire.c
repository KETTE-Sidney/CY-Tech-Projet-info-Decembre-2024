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

AVLNode* CreerNoeud(int stationID, long capacite, long consumption) {
    AVLNode *noeud = (AVLNoeud*)malloc(sizeof(AVLNoeud));
    if (!noeud) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    noeud->stationID = stationID;
    noeud->capacite = capacite;
    noeud->consommation = conssomation;
    noeud->taille = 1;
    noeud->gauche = noeud->droit = NULL;
    return noeud;
}

// ca donne la hauteur du noeud
int avoirTaille(AVLNoeud *noeud) {
    return noeud ? noeud->taille : 0;
}

int equilibre(AVLNoeud *noeud) {
    if (!noeud) return 0;
    return avoirTaille(noeud->gauche) - avoirTaille(noeud->droite);
}

// Rotation SD
AVLNode* rotationDroite(AVLNoeud *y) {
    AVLNoeud *x = y->gauche;
    AVLNoeud *T = x->droite

    x->droite = y;
    y->gauche = T;

    y->taille = 1 + (avoirTaille(y->gauche) > avoirTaille(y->droite) ? avoirTaille(y->gauche) : avoirTaille(y->droite));


    return x;
}

// Rotation SG
AVLNode* rotationGauche(AVLNoeud *x) {
    AVLNoeud *y = x->droite;
    AVLNoeud *T = y->gauche;

    y->gauche = x;
    x->droite = T;

    x->taille = 1 + (avoirTaille(x->gauche) > avoirTaille(x->droite) ? avoirTaille(x->gauche) : avoirTaille(x->droite));
    y->taille = 1 + (avoirTaille(y->gauche) > avoirTaille(y->droite) ? avoirTaille(y->gauche) : avoirTaille(y->droite));

    return y;
}

AVLNoeud* insererNeoud(AVLNoeud *noeud, int stationID, long capacite, long consommation) {
    if (!noeud) return creerNoeud(stationID, capacite, consommation);

    if (stationID < noeud->stationID)
        noeud->gauche = insererNoeud(noeud->gauche, stationID, capacite, consommation);
    else if (stationID > noeud->stationID)
        noeud->droite = insererNoeud(noeud->droite, stationID, capacite, consommation);
    else { // Si le nœud existe déjà, on met à jour sa consommation
        noeud->consommation += consommation;
        return noeud;
    }

    noeud->taille = 1 + (avoirTaille(noeud->gauche) > avoirTaille(noeud->droite) ? avoirTaille(noeud->gauche) : avoirTaille(noeud->droite));

    int h = equilibre(node);

    // Rotation pour équilibrer
    if (h > 1 && stationID < noeud->gauche->stationID)
        return rotationDroite(noeud);

    if (h < -1 && stationID > noeud->droite->stationID)
        return roationGauche(node);

    if (h > 1 && stationID > node->gauche->stationID) {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    if (h < -1 && stationID < noeud->droite->stationID) {
        noeud->droite = rotationDroite(noeud->droite)
        return rotationGauche(noeud);
    }

    return noeud;
}

// En infixe
void parcoursInfixe(AVLNoeud *racine) {
    if (racine) {
        parcoursInfixe(racine->gauche);
        printf("Station ID: %d, Capacite: %ld, Consommation: %ld\n", racine->stationID, racine->capacite, racine->consommation);
        parcoursInfixe(racine->droite);
    }
}

// c est pour liberer l'AVL
void freeAVL(AVLNoeud *racine) {
    if (racine) {
        freeAVL(racine->gauche);
        freeAVL(racine->droite);
        free(racine);
    }
}


// traitement csv

void lireCSV(const char *filenom, AVLNoeud **racine) {
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
//ajouter comentaire sur cette partie... 
    AVLNode *racine = NULL;

    lireCSV(argv[1], &root);

    printf("Contenu de l'arbre AVL :\n");
    parcoursInfixe(root);
    freeAVL(root);

    return 0;
}
