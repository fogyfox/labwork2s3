#ifndef SET_H
#define SET_H

#include <vector>
#include <string>

struct NodeSet {
    int key;
    NodeSet* next;
};

struct Set {
    NodeSet** buckets;
    int tableSize;
    int itemCount;
};

//базовые операции множества
void createSet(Set* set, int initialSize = 101);
void destroySet(Set* set);
bool insert(Set* set, int key);
bool contains(const Set* set, int key);
bool remove(Set* set, int key);
int size(const Set* set);
bool empty(const Set* set);
void clear(Set* set);

//операции с множествами
Set unionSets(const Set* set1, const Set* set2);
Set intersection(const Set* set1, const Set* set2);
Set difference(const Set* set1, const Set* set2);
bool isSubset(const Set* set1, const Set* set2);

//вспомогательные функции
void printSet(const Set* set);
void saveSetToFile(const Set* set, const std::string& filename);
void loadSetFromFile(Set* set, const std::string& filename);

//новые функции для работы с числами и разбиения на подмножества
std::vector<int> setToVector(const Set* set);
Set vectorToSet(const std::vector<int>& vec);
bool partitionSetImproved(const Set* set, int subsetSum, std::vector<std::vector<int>>& result);

#endif