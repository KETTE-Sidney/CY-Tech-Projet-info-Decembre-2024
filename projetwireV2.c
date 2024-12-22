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

    y->hauteur = 1 + (gethauteur(y->gauche) > gethauteur(y->droite) ? gethauteur(y->gauche) : gethauteur(y->droite));
    x->hauteur = 1 + (gethauteur(x->gauche) > gethauteur(x->droite) ? gethauteur(x->gauche) : gethauteur(x->droite));

    return x;
}

// Rotation gauche
AVLNoeud* rotationGauche(AVLNoeud *x) {
    AVLNoeud *y = x->droite;
    AVLNoeud *T = y->gauche;

    y->gauche = x;
    x->droite = T;

    x->hauteur = 1 + (gethauteur(x->gauche) > gethauteur(x->droite) ? gethauteur(x->gauche) : gethauteur(x->droite));
    y->hauteur = 1 + (gethauteur(y->gauche) > gethauteur(y->droite) ? gethauteur(y->gauche) : gethauteur(y->droite));

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

// fonction detecter sur ou sous production
const char* verifierProduction(long capacite, long consommation) {
    if (capacite > consommation) {
        return "Sous-production";
    } else if (capacite < consommation) { //
        return "Surproduction";
    } else {
        return "Équilibrée";
    }
}
 
// Calculer la somme des consommations dans l'arbre
long sommeConsommations(AVLNoeud *racine) {
    if (!racine) return 0;
    return racine->consommation + sommeConsommations(racine->gauche) + sommeConsommations(racine->droite);
}

// Calculer la somme des capacités dans l'arbre
long sommeCapacites(AVLNoeud *racine) {
    if (!racine) return 0;
    return racine->capacite + sommeCapacites(racine->gauche) + sommeCapacites(racine->droite);
}

// Compter le nombre de nœuds dans l'arbre
int compterNoeuds(AVLNoeud *racine) {
    if (!racine) return 0;
    return 1 + compterNoeuds(racine->gauche) + compterNoeuds(racine->droite);
}

// Rechercher un noeud spécifique
AVLNoeud* rechercherStation(AVLNoeud *racine, int stationID) {
    if (!racine || racine->stationID == stationID)
        return racine;
    if (stationID < racine->stationID)
        return rechercherStation(racine->gauche, stationID);
    return rechercherStation(racine->droite, stationID);
}

void parcoursInfixe(AVLNoeud *racine, FILE* fichier) {
    if (!racine) return;
        parcoursInfixe(racine->gauche, fichier);
        printf("Station ID: %d, Capacite: %ld, Consommation: %ld\n", racine->stationID, racine->capacite, racine->consommation);
        parcoursInfixe(racine->droite, fichier);
    }


void freeAVL(AVLNoeud *racine) {
    if (!racine) return;
        freeAVL(racine->gauche);
        freeAVL(racine->droite);
        free(racine);
}

// Lire un fichier CSV et remplir l'arbre AVL
AVLNoeud* lireCSV(const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "r");
    if (!fichier) {
        perror("Échec de l'ouverture du fichier CSV");
        exit(1);
    }

    AVLNoeud* racine = NULL;
    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        int stationID;
        long capacite = 0, consommation = 0;
        // Lecture des colonnes du fichier CSV
        ligne[strcspn(ligne, "\n")] = '\0';  
        // **Suppression du saut de ligne**

        sscanf(ligne, "%d:%ld:%ld", &stationID, &capacite, &consommation);  // **Utilisation de ':' comme séparateur**
        if (stationID == 0) continue;  

        racine = insererNoeud(racine, stationID, capacite, consommation);
    }

    fclose(fichier);
    return racine;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {  
        fprintf(stderr, "Usage: %s <fichier CSV> <output_file> <type_station>\n", argv[0]);
        return 1;
    }
const char* type_station = argv[3];  // **Nouvel argument pour filtrer les types de stations**


    const char* input = argv[1];
    const char* output = argv[2];
    const char *station_type = argv[3];

    AVLNoeud* racine = lireCSV(input);
    if (!racine) {  
        fprintf(stderr, "Aucune donnée valide trouvée dans le fichier CSV.\n");
        return 1;
    }


     // Calculer les statistiques
    long totalCapacite = sommeCapacites(racine);
    long totalConsommation = sommeConsommations(racine);

    // Afficher les résultats
    printf("\nRésumé:\n");
    printf("Capacité totale: %ld\n", totalCapacite);
    printf("Consommation totale: %ld\n", totalConsommation);
    printf("Production globale: %s\n", verifierProduction(totalCapacite, totalConsommation));


    FILE* fichierResultat = fopen(output, "w");
    if (!fichierResultat) {
        perror("Échec de l'ouverture du fichier de sortie");
        freeAVL(racine);
        exit(1);
    }

    fprintf(fichierResultat, "StationID:Capacite:Consommation\n");
    parcoursInfixe(racine, fichierResultat);

    fclose(fichierResultat);
    freeAVL(racine);

    printf("Résultats sauvegardés dans %s\n", output);

    return 0;
}
