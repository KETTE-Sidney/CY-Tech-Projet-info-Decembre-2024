#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

AVLNode* createNode(int stationID, long capacity, long consumption) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    node->stationID = stationID;
    node->capacity = capacity;
    node->consumption = consumption;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));

    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));

    return y;
}

AVLNode* insertNode(AVLNode* node, int stationID, long capacity, long consumption) {
    if (!node) return createNode(stationID, capacity, consumption);

    if (stationID < node->stationID)
        node->left = insertNode(node->left, stationID, capacity, consumption);
    else if (stationID > node->stationID)
        node->right = insertNode(node->right, stationID, capacity, consumption);
    else {
        node->consumption += consumption;
        return node;
    }

    node->height = 1 + (getHeight(node->left) > getHeight(node->right) ? getHeight(node->left) : getHeight(node->right));

    int balance = getBalance(node);

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

void inorderTraversal(AVLNode* root, FILE* file) {
    if (!root) return;
    inorderTraversal(root->left, file);
    fprintf(file, "%d:%ld:%ld\n", root->stationID, root->capacity, root->consumption);
    inorderTraversal(root->right, file);
}

void freeTree(AVLNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
