#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct NodeBST {
    int key;
    NodeBST* left;
    NodeBST* right;
    NodeBST(int k) : key(k), left(nullptr), right(nullptr) {}
};

//создание узла
NodeBST* CreateNodeFBT(int key) {
    return new NodeBST(key);
}

//добавление узла в BST
NodeBST* InsertNode(NodeBST* root, int key) {
    if (!root) return CreateNodeFBT(key);

    if (key < root->key)
        root->left = InsertNode(root->left, key);
    else if (key > root->key)
        root->right = InsertNode(root->right, key);
    
    return root;
}

//обход inorder для получения отсортированных значений
void InorderTraversal(NodeBST* root, vector<int>& values) {
    if (!root) return;
    InorderTraversal(root->left, values);
    values.push_back(root->key);
    InorderTraversal(root->right, values);
}

//восстановление правильных значений в дереве
void RestoreValues(NodeBST* root, vector<int>& correctValues, int& index) {
    if (!root) return;
    RestoreValues(root->left, correctValues, index);
    root->key = correctValues[index++];
    RestoreValues(root->right, correctValues, index);
}

//основная функция восстановления BST
NodeBST* RestoreBST(NodeBST* root) {
    if (!root) return root;
    
    //1.получаем текущие значения в дереве (inorder обход)
    vector<int> currentValues;
    InorderTraversal(root, currentValues);
    
    //2.сортируем значения (это будет правильный порядок для BST)
    vector<int> correctValues = currentValues;
    sort(correctValues.begin(), correctValues.end());
    
    //3.восстанавливаем значения в дереве, сохраняя структуру
    int index = 0;
    RestoreValues(root, correctValues, index);
    
    return root;
}

//вспомогательные функции для демонстрации

//обход inorder
void InorderPrint(NodeBST* root) {
    if (!root) return;
    InorderPrint(root->left);
    cout << root->key << " ";
    InorderPrint(root->right);
}


//создание тестового дерева с двумя поменянными узлами
NodeBST* CreateTestTree() {
    // Создаем корректное BST: 
    //       4
    //      / \
    //     2   6
    //    / \ / \
    //   1  3 5  7
    
    NodeBST* root = nullptr;
    root = InsertNode(root, 4);
    root = InsertNode(root, 2);
    root = InsertNode(root, 6);
    root = InsertNode(root, 1);
    root = InsertNode(root, 3);
    root = InsertNode(root, 5);
    root = InsertNode(root, 7);
    
    //меняем два узла местами (например, 3 и 6)
    //ищем узлы и меняем их значения
    NodeBST* node3 = root->left->right;  // узел со значением 3
    NodeBST* node6 = root->right;        // узел со значением 6
    
    swap(node3->key, node6->key);
    
    return root;
}

int main() {
    //создаем дерево с двумя поменянными узлами
    NodeBST* root = CreateTestTree();
    
    cout << "Дерево ДО восстановления:" << endl;
    cout << "Inorder обход: ";
    InorderPrint(root);
    cout << endl;
    cout << endl;
    
    //восстанавливаем дерево
    root = RestoreBST(root);
    cout << "Дерево ПОСЛЕ восстановления:" << endl;
    cout << "Inorder обход: ";
    InorderPrint(root);
    cout << endl;
    
    return 0;
}