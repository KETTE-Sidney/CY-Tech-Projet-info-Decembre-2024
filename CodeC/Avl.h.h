#ifndef AVL_H
#define AVL_H

#include <stdio.h> // Ajout de cette ligne

typedef struct AVLNode {
    int stationID;
    long capacity;
    long consumption;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

AVLNode* createNode(int stationID, long capacity, long consumption);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertNode(AVLNode* node, int stationID, long capacity, long consumption);
void inorderTraversal(AVLNode* root, FILE* file);
void freeTree(AVLNode* root);

#endif
