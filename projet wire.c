#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AVLNode {
    int stationID;
    long capacity;   // Capacité en kWh
    long consumption; // Consommation totale des consommateurs
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

AVLNode* createNode(int stationID, long capacity, long consumption) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    node->stationID = stationID;
    node->capacity = capacity;
    node->consumption = consumption;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

// ca donne la hauteur du noeud
int getHeight(AVLNode *node) {
    return node ? node->height : 0;
}

int getBalanceFactor(AVLNode *node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Rotation SD
AVLNode* rotateRight(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));

    return x;
}

// Rotation SG
AVLNode* rotateLeft(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));

    return y;
}

AVLNode* insertNode(AVLNode *node, int stationID, long capacity, long consumption) {
    if (!node) return createNode(stationID, capacity, consumption);

    if (stationID < node->stationID)
        node->left = insertNode(node->left, stationID, capacity, consumption);
    else if (stationID > node->stationID)
        node->right = insertNode(node->right, stationID, capacity, consumption);
    else { // Si le nœud existe déjà, on met à jour sa consommation
        node->consumption += consumption;
        return node;
    }

    node->height = 1 + (getHeight(node->left) > getHeight(node->right) ? getHeight(node->left) : getHeight(node->right));

    int balance = getBalanceFactor(node);

    // Rotation pour équilibrer
    if (balance > 1 && stationID < node->left->stationID)
        return rotateRight(node);

    if (balance < -1 && stationID > node->right->stationID)
        return rotateLeft(node);

    if (balance > 1 && stationID > node->left->stationID) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && stationID < node->right->stationID) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// En infixe
void inorderTraversal(AVLNode *root) {
    if (root) {
        inorderTraversal(root->left);
        printf("Station ID: %d, Capacity: %ld, Consumption: %ld\n", root->stationID, root->capacity, root->consumption);
        inorderTraversal(root->right);
    }
}

// c est pour liberer l'AVL
void freeAVL(AVLNode *root) {
    if (root) {
        freeAVL(root->left);
        freeAVL(root->right);
        free(root);
    }
}


// traitement csv

void readCSV(const char *filename, AVLNode **root) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int stationID;
        long capacity = 0, consumption = 0;

        sscanf(line, "%d;%ld;%ld", &stationID, &capacity, &consumption);
        *root = insertNode(*root, stationID, capacity, consumption);
    }

    fclose(file);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier CSV>\n", argv[0]);
        return 1;
    }
//ajouter comentaire sur cette partie... 
    AVLNode *root = NULL;

    readCSV(argv[1], &root);

    printf("Contenu de l'arbre AVL :\n");
    inorderTraversal(root);
    freeAVL(root);

    return 0;
}
