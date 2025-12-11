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

//новая функция для вывода всех различных подмассивов
void printAllSubarrays(Array* arr) {
    int n = arr->size;
    
    cout << "Все различные подмассивы:" << endl;
    
    //пустой подмассив
    cout << "{}" << endl;
    
    //генерируем все возможные подмассивы
    for (int start = 0; start < n; start++) {
        for (int end = start; end < n; end++) {
            cout << "{";
            for (int i = start; i <= end; i++) {
                cout << arr->data[i].value;
                if (i < end) {
                    cout << ", ";
                }
            }
            cout << "}" << endl;
        }
    }
}