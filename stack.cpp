#include<iostream>
#include<string>
#include "stack.h"
using namespace std;


bool empty(stack* st){
    return st -> head == nullptr;    
}

//инициализация стека
void createStack(stack* st){
    st -> head = nullptr; //вершина в nillptr
    st -> size = 0;

}

//создание нового узла(ноды)
NodeS* createNode(const string& key){
    NodeS* newNode = new NodeS;
    newNode -> key = key; //задаём значение
    newNode -> next = nullptr;
    return newNode; //возвращаем указатель
}

//добавление эл-та в стек
void push(stack* st, const string& data){
    NodeS* newNode = createNode(data);
    newNode -> next = st -> head; //новый эл-т указывает на бывшую вершину стека
    st -> head = newNode; //новая нода - вершина стека
    st -> size++;
}

NodeS* pop(stack* st){
   if (empty(st)) {
        cout << "Стек пустой" << endl;
        return nullptr;
    }
    
    NodeS* el = st->head;
    st->head = st->head->next;
    
    st->size--;
    
    // Отсоединяем элемент от стека перед возвратом
    el->next = nullptr;
    return el;
}

void printStack(stack* st){
    if (empty(st)){
        cout << "Стек пустой" << endl;
        return;
    }

    NodeS* el = st -> head;
    cout << "Вывод стека: ";
    while (el != nullptr){
        cout << el -> key << " ";
        el = el -> next; //переход к след. эл-ту
    }
    cout << endl;
}

string SgetInx(stack* st, int inx){
    if (inx < 0 || inx >= st -> size){
        cout << "Выход за диапозон" << endl;
        return "";
    }
    NodeS* el = st -> head;
    for(int i = 0; i < inx; i++){
        el = el -> next;
    }
    return el -> key;//получаес значение по индексу
}
