#ifndef ARRAYOP_H
#define ARRAYOP_H

#include <string>

struct NodeA{
    std::string value;
};

struct Array{
    NodeA* data;
    int size;
    int capacity;
};

void createArray(Array* arr);
NodeA* createNodeAr(const std::string& value);
void pushBack(Array* arr, const std::string& value);
void addInx(Array* arr, const std::string& value, int inx);
void printArray(Array* arr);
void changeInx(Array* arr, const std::string& newValue, int inx);
std::string getInx(Array* arr, int inx);
int sizeArr(Array* arr);
std::string removeElArr(Array* arr, int inx);
void printAllSubarrays(Array* arr); // Новая функция для вывода всех подмассивов

#endif