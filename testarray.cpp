#include <iostream>
#include "arrayOp.h"

using namespace std;

int main() {
    Array arr;
    createArray(&arr);
    
    //добавляем элементы в массив
    pushBack(&arr, "x");
    pushBack(&arr, "y");
    pushBack(&arr, "z");
    
    printArray(&arr);
    cout << endl;
    
    //выводим все подмассивы
    printAllSubarrays(&arr);
    
    return 0;
}