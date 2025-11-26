#ifndef FULLBINARYTREE_H
#define FULLBINARYTREE_H

#include <vector>
using namespace std;

struct NodeBST {
    int key;
    NodeBST* left;
    NodeBST* right;
    NodeBST(int k) : key(k), left(nullptr), right(nullptr) {}
};

NodeBST* CreateNodeFBT(int key);
NodeBST* InsertNode(NodeBST* root, int key);
NodeBST* RestoreBST(NodeBST* root);
void InorderTraversal(NodeBST* root, vector<int>& values);
void RestoreValues(NodeBST* root, vector<int>& correctValues, int& index);
void InorderPrint(NodeBST* root);
void LevelOrderPrint(NodeBST* root);
NodeBST* CreateTestTree();

#endif