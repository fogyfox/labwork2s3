#ifndef STACK_H
#define STACK_H
#include <string>

using namespace std;

struct NodeS{
    string key; //значение
    NodeS* next; //указатель на след. элемент
};

struct stack
{
    NodeS* head; //вершина стека
    int size;
};

bool empty(stack* st);
void createStack(stack* stackForm);
NodeS* createNode(const std::string& key);
void push(stack* st, const std::string& data);  // Добавить const
NodeS* pop(stack* st);
void printStack(stack* st);
string SgetInx(stack* st, int inx);

#endif