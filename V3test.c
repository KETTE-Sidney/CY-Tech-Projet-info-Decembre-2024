#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition de la structure d'un nœud de l'arbre AVL
typedef struct AVLNoeud {
    int stationID;
    long capacite;
    long consommation;
    struct AVLNoeud *gauche;
    struct AVLNoeud *droit;
    int hauteur;
} AVLNoeud;

// Fonction pour calculer la hauteur d'un nœud
int hauteur(AVLNoeud *noeud) {
    return noeud ? noeud->hauteur : 0;
}

// Fonction pour créer un nouveau nœud
AVLNoeud *nouveauNoeud(int stationID, long capacite, long consommation) {
    AVLNoeud *noeud = (AVLNoeud *)malloc(sizeof(AVLNoeud));
    if (!noeud) {
        fprintf(stderr, "Erreur : allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    noeud->stationID = stationID;
    noeud->capacite = capacite;
    noeud->consommation = consommation;
    noeud->gauche = noeud->droit = NULL;
    noeud->hauteur = 1;
    return noeud;
}

// Fonction pour effectuer une rotation droite
AVLNoeud *rotationDroite(AVLNoeud *y) {
    AVLNoeud *x = y->gauche;
    AVLNoeud *T2 = x->droit;

    x->droit = y;
    y->gauche = T2;

    y->hauteur = 1 + (hauteur(y->gauche) > hauteur(y->droit) ? hauteur(y->gauche) : hauteur(y->droit));
    x->hauteur = 1 + (hauteur(x->gauche) > hauteur(x->droit) ? hauteur(x->gauche) : hauteur(x->droit));

    return x;
}

// Fonction pour effectuer une rotation gauche
AVLNoeud *rotationGauche(AVLNoeud *x) {
    AVLNoeud *y = x->droit;
    AVLNoeud *T2 = y->gauche;

    y->gauche = x;
    x->droit = T2;

    x->hauteur = 1 + (hauteur(x->gauche) > hauteur(x->droit) ? hauteur(x->gauche) : hauteur(x->droit));
    y->hauteur = 1 + (hauteur(y->gauche) > hauteur(y->droit) ? hauteur(y->gauche) : hauteur(y->droit));

    return y;
}

// Fonction pour obtenir le facteur d'équilibre
int facteurEquilibre(AVLNoeud *noeud) {
    return noeud ? hauteur(noeud->gauche) - hauteur(noeud->droit) : 0;
}

// Fonction pour insérer un nœud dans l'arbre AVL
AVLNoeud *insererNoeud(AVLNoeud *racine, int stationID, long capacite, long consommation) {
    if (!racine) {
        return nouveauNoeud(stationID, capacite, consommation);
    }

    if (stationID < racine->stationID) {
        racine->gauche = insererNoeud(racine->gauche, stationID, capacite, consommation);
    } else if (stationID > racine->stationID) {
        racine->droit = insererNoeud(racine->droit, stationID, capacite, consommation);
    } else {
        // Les duplications ne sont pas autorisées
        return racine;
    }

    racine->hauteur = 1 + (hauteur(racine->gauche) > hauteur(racine->droit) ? hauteur(racine->gauche) : hauteur(racine->droit));
    int equilibre = facteurEquilibre(racine);

    // Rotation gauche-droite si déséquilibré
    if (equilibre > 1 && stationID < racine->gauche->stationID) {
        return rotationDroite(racine);
    }

    if (equilibre < -1 && stationID > racine->droit->stationID) {
        return rotationGauche(racine);
    }

    if (equilibre > 1 && stationID > racine->gauche->stationID) {
        racine->gauche = rotationGauche(racine->gauche);
        return rotationDroite(racine);
    }

    if (equilibre < -1 && stationID < racine->droit->stationID) {
        racine->droit = rotationDroite(racine->droit);
        return rotationGauche(racine);
    }

    return racine;
}

// Fonction pour calculer les capacités totales
long calculerCapaciteTotale(AVLNoeud *noeud) {
    if (!noeud) return 0;
    return noeud->capacite + calculerCapaciteTotale(noeud->gauche) + calculerCapaciteTotale(noeud->droit);
}

// Fonction pour afficher l'arbre AVL
void afficherArbre(AVLNoeud *noeud, int niveau) {
    if (!noeud) return;

    afficherArbre(noeud->droit, niveau + 1);

    for (int i = 0; i < niveau; i++) printf("    ");
    printf("StationID=%d, Capacité=%ld, Consommation=%ld\n", noeud->stationID, noeud->capacite, noeud->consommation);

    afficherArbre(noeud->gauche, niveau + 1);
}

// Fonction principale
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier CSV>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    AVLNoeud *racine = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        int stationID;
        long capacite, consommation;
        if (sscanf(line, "%d;%ld;%ld", &stationID, &capacite, &consommation) != 3) {
            fprintf(stderr, "Ligne ignorée : %s\n", line);
            continue;
        }
        racine = insererNoeud(racine, stationID, capacite, consommation);
    }

    fclose(file);

    printf("Arbre AVL :\n");
    afficherArbre(racine, 0);

    printf("Capacité totale : %ld\n", calculerCapaciteTotale(racine));

    return 0;
}
