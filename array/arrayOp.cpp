#include<iostream>
#include"arrayOp.h"
#include <string>
#include <vector>
using namespace std;

bool empty(Array* arr){
    return arr->size == 0;    
}

//создание массива
void createArray(Array* arr){
    arr->capacity = 1;
    arr->data = new NodeA[arr->capacity];
    arr->size = 0;
}

NodeA* createNodeAr(const string& value){
    NodeA* newNode = new NodeA;
    newNode->value = value; //задаём значение
    return newNode; //возвращаем указатель
}

void pushBack(Array* arr, const string& value){
    if (arr->size >= arr->capacity) {//проверяем нужно ли увеличить емкость
        arr->capacity *= 2;
        NodeA* newData = new NodeA[arr->capacity];
        
        //копируем существующие элементы
        for (int i = 0; i < arr->size; i++) {
            newData[i] = arr->data[i];
        }
        
        delete[] arr->data;
        arr->data = newData;
    }
    
    NodeA newNode;
    newNode.value = value;
    arr->data[arr->size] = newNode;
    arr->size++;
}

void addInx(Array* arr, const string& value, int inx){
    if (inx < 0 || inx > arr->size){
        cout << "Выход за диапазон" << endl;
        return;
    }
    
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        NodeA* newData = new NodeA[arr->capacity];
        
        for (int i = 0; i < arr->size; i++) {
            newData[i] = arr->data[i];
        }
        
        delete[] arr->data;
        arr->data = newData;
    }
    
    //сдвигаем элементы вправо, начиная с конца
    for (int i = arr->size; i > inx; i--) {
        arr->data[i] = arr->data[i-1];
    }
    
    NodeA newNode;
    newNode.value = value;
    arr->data[inx] = newNode;
    arr->size++;
}

void printArray(Array* arr){
    if (empty(arr)){
        cout << "Массив пустой" << endl;
        return;
    }

    cout << "Вывод массива: ";
    for (int i = 0; i < arr->size; i++) {
        cout << arr->data[i].value << " ";
    }
    cout << endl;
}

//получение эл-та по индексу
string getInx(Array* arr, int inx){
    if (inx < 0 || inx >= arr->size){
        cout << "Выход за диапазон" << endl;
        return "";
    }
    return arr->data[inx].value;
}

//замена эл-та по индексу
void changeInx(Array* arr, const string& newValue, int inx){
    if (inx < 0 || inx >= arr->size){
        cout << "Выход за диапазон" << endl;
        return;
    }
    arr->data[inx].value = newValue;
}

int sizeArr(Array* arr){
    return arr->size;
}

string removeElArr(Array* arr, int inx){
    if (inx < 0 || inx >= arr->size){
        cout << "Выход за диапазон" << endl;
        return "";
    }

    //сохраняем значение удаляемого элемента
    string removedValue = arr->data[inx].value;
    
    //сдвигаем элементы влево, начиная с удаляемого индекса
    for (int i = inx; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i+1];
    }
    
    arr->size--;
    return removedValue;
}

void printAllSubarrays(Array* arr) {
    int n = arr->size;
    int total = 1 << n; //двоичная форма 2^n
    
    cout << "Все различные подмножества:" << endl;
    
    for (int mask = 0; mask < total; mask++) {
        cout << "{";
        bool first = true;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) { // двоичная форма 000, 001 и т.д
                if (!first) cout << ", ";
                cout << arr->data[i].value;
                first = false;
            }
        }
        cout << "}" << endl;
    }
}


//0 - 000 - нет - {}
//1 - 001 - data[2] - {z}
//2 - 010 - data[1] - {y}
//3 - 011 - data[1], data[2] - {y, z}
//4 - 100 - data[0] - {x}
//5 - 101 - data[0], data[2] - {x, z}
//6 - 110 - data[0], data[1] - {x, y}
//7 - 111 - все - {x, y, z}