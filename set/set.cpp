#include "set.h"
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

//хэш-функция для целых чисел
int hashFunction(int key, int tableSize) {
    hash<int> hasher;
    return hasher(key) % tableSize;
}

//создание множества
void createSet(Set* set, int initialSize) {
    set->tableSize = initialSize;
    set->itemCount = 0;
    set->buckets = new NodeSet*[initialSize];
    for (int i = 0; i < initialSize; i++) {
        set->buckets[i] = nullptr;
    }
}

//уничтожение множества
void destroySet(Set* set) {
    clear(set);
    delete[] set->buckets;
    set->buckets = nullptr;
    set->tableSize = 0;
    set->itemCount = 0;
}

//проверка на необходимость рехеширования
void rehashIfNeeded(Set* set) {
    double loadFactor = static_cast<double>(set->itemCount) / set->tableSize;
    if (loadFactor > 0.7) {
        int newSize = set->tableSize * 2;
        NodeSet** newBuckets = new NodeSet*[newSize];
        
        for (int i = 0; i < newSize; i++) {
            newBuckets[i] = nullptr;
        }
        
        //перехэширование всех элементов
        for (int i = 0; i < set->tableSize; i++) {
            NodeSet* current = set->buckets[i];
            while (current != nullptr) {
                NodeSet* next = current->next;
                int newIndex = hashFunction(current->key, newSize);
                current->next = newBuckets[newIndex];
                newBuckets[newIndex] = current;
                current = next;
            }
        }
        
        delete[] set->buckets;
        set->buckets = newBuckets;
        set->tableSize = newSize;
    }
}

//добавление элемента
bool insert(Set* set, int key) {
    if (contains(set, key)) {
        return false; //элемент уже существует
    }
    
    int index = hashFunction(key, set->tableSize);
    NodeSet* newNode = new NodeSet{key, set->buckets[index]};
    set->buckets[index] = newNode;
    set->itemCount++;
    
    rehashIfNeeded(set);
    return true;
}

//проверка наличия элемента
bool contains(const Set* set, int key) {
    int index = hashFunction(key, set->tableSize);
    NodeSet* current = set->buckets[index];
    
    while (current != nullptr) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    return false;
}

//удаление элемента
bool remove(Set* set, int key) {
    int index = hashFunction(key, set->tableSize);
    NodeSet* current = set->buckets[index];
    NodeSet* prev = nullptr;
    
    while (current != nullptr) {
        if (current->key == key) {
            if (prev == nullptr) {
                set->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            set->itemCount--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

//получение размера множества
int size(const Set* set) {
    return set->itemCount;
}

//проверка пустоты множества
bool empty(const Set* set) {
    return set->itemCount == 0;
}

//очистка множества
void clear(Set* set) {
    for (int i = 0; i < set->tableSize; i++) {
        NodeSet* current = set->buckets[i];
        while (current != nullptr) {
            NodeSet* next = current->next;
            delete current;
            current = next;
        }
        set->buckets[i] = nullptr;
    }
    set->itemCount = 0;
}

//объединение множеств
Set unionSets(const Set* set1, const Set* set2) {
    Set result;
    createSet(&result, set1->tableSize);
    
    //добавляем все элементы из первого множества
    for (int i = 0; i < set1->tableSize; i++) {
        NodeSet* current = set1->buckets[i];
        while (current != nullptr) {
            insert(&result, current->key);
            current = current->next;
        }
    }
    
    //добавляем все элементы из второго множества
    for (int i = 0; i < set2->tableSize; i++) {
        NodeSet* current = set2->buckets[i];
        while (current != nullptr) {
            insert(&result, current->key);
            current = current->next;
        }
    }
    
    return result;
}

//пересечение множеств
Set intersection(const Set* set1, const Set* set2) {
    Set result;
    createSet(&result, set1->tableSize);
    
    //проходим по меньшему множеству для оптимизации
    const Set* smaller = (set1->itemCount < set2->itemCount) ? set1 : set2;
    const Set* larger = (set1->itemCount < set2->itemCount) ? set2 : set1;
    
    for (int i = 0; i < smaller->tableSize; i++) {
        NodeSet* current = smaller->buckets[i];
        while (current != nullptr) {
            if (contains(larger, current->key)) {
                insert(&result, current->key);
            }
            current = current->next;
        }
    }
    
    return result;
}

//разность множеств
Set difference(const Set* set1, const Set* set2) {
    Set result;
    createSet(&result, set1->tableSize);
    
    for (int i = 0; i < set1->tableSize; i++) {
        NodeSet* current = set1->buckets[i];
        while (current != nullptr) {
            if (!contains(set2, current->key)) {
                insert(&result, current->key);
            }
            current = current->next;
        }
    }
    
    return result;
}

//проверка подмножества
bool isSubset(const Set* set1, const Set* set2) {
    //set1 является подмножеством set2, если все элементы set1 содержатся в set2
    for (int i = 0; i < set1->tableSize; i++) {
        NodeSet* current = set1->buckets[i];
        while (current != nullptr) {
            if (!contains(set2, current->key)) {
                return false;
            }
            current = current->next;
        }
    }
    return true;
}

//вывод множества
void printSet(const Set* set) {
    cout << "{";
    bool first = true;
    
    for (int i = 0; i < set->tableSize; i++) {
        NodeSet* current = set->buckets[i];
        while (current != nullptr) {
            if (!first) {
                cout << ", ";
            }
            cout << current->key;
            first = false;
            current = current->next;
        }
    }
    cout << "}" << endl;
}

//сохранение множества в файл
void saveSetToFile(const Set* set, const string& filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Ошибка открытия файла для записи: " << filename << endl;
        return;
    }
    
    for (int i = 0; i < set->tableSize; i++) {
        NodeSet* current = set->buckets[i];
        while (current != nullptr) {
            file << current->key << endl;
            current = current->next;
        }
    }
    
    file.close();
}

//загрузка множества из файла
void loadSetFromFile(Set* set, const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Ошибка открытия файла для чтения: " << filename << endl;
        return;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            insert(set, stoi(line));
        }
    }
    
    file.close();
}

//преобразование множества в вектор
vector<int> setToVector(const Set* set) {
    vector<int> result;
    for (int i = 0; i < set->tableSize; i++) {
        NodeSet* current = set->buckets[i];
        while (current != nullptr) {
            result.push_back(current->key);
            current = current->next;
        }
    }
    return result;
}

//преобразование вектора в множество
Set vectorToSet(const vector<int>& vec) {
    Set result;
    createSet(&result);
    for (int num : vec) {
        insert(&result, num);
    }
    return result;
}

//рекурсивная функция backtracking для разбиения
bool partitionBacktrack(vector<int>& nums, vector<vector<int>>& subsets, 
                       vector<int>& subsetSums, int target, int index) {
    if (index == nums.size()) {
        //проверяем, что все подмножества имеют целевую сумму
        for (int sum : subsetSums) {
            if (sum != target) return false;
        }
        return true;
    }
    
    int num = nums[index];
    
    //пробуем добавить число в каждое подмножество
    for (int i = 0; i < subsets.size(); i++) {
        if (subsetSums[i] + num <= target) {
            //добавляем число в подмножество
            subsets[i].push_back(num);
            subsetSums[i] += num;
            
            if (partitionBacktrack(nums, subsets, subsetSums, target, index + 1)) {
                return true;
            }
            
            // Откатываем изменения
            subsets[i].pop_back();
            subsetSums[i] -= num;
            
            // Если подмножество пустое, не пробуем следующие пустые подмножества
            if (subsetSums[i] == 0) break;
        }
    }
    
    return false;
}

//основная функция разбиения множества на подмножества с заданной суммой
bool partitionSetImproved(const Set* set, int subsetSum, vector<vector<int>>& result) {
    vector<int> nums = setToVector(set);
    int totalSum = 0;
    for (int num : nums) {
        totalSum += num;
    }
    
    //проверяем возможность разбиения
    if (totalSum % subsetSum != 0) {
        cout << "Невозможно разбить множество: общая сумма " << totalSum 
             << " не делится нацело на " << subsetSum << endl;
        return false;
    }
    
    int k = totalSum / subsetSum; //количество подмножеств
    
    //сортируем в порядке убывания для оптимизации
    sort(nums.begin(), nums.end(), greater<int>());
    
    //проверяем, что максимальный элемент не превышает целевую сумму
    if (nums[0] > subsetSum) {
        cout << "Невозможно разбить множество: элемент " << nums[0] 
             << " превышает целевую сумму " << subsetSum << endl;
        return false;
    }
    
    //инициализируем подмножества
    vector<vector<int>> subsets(k);
    vector<int> subsetSums(k, 0);
    
    if (partitionBacktrack(nums, subsets, subsetSums, subsetSum, 0)) {
        result = subsets;
        return true;
    }
    
    cout << "Невозможно разбить множество на " << k << " подмножеств с суммой " << subsetSum << endl;
    return false;
}