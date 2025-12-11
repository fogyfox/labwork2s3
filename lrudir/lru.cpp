#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class LRUCache {
private:
    int capacity;
    list<pair<int, int>> cache;
    unordered_map<int, list<pair<int, int>>::iterator> keyMap;

public:
    LRUCache(int cap) {
        capacity = cap;
    }

    int get(int key) {
        if (keyMap.find(key) == keyMap.end()) {
            return -1;
        }
        
        auto it = keyMap[key];
        int value = it->second;
        cache.erase(it);
        cache.push_front({key, value});
        keyMap[key] = cache.begin();
        
        return value;
    }

    void set(int key, int value) {
        if (keyMap.find(key) != keyMap.end()) {
            auto it = keyMap[key];
            cache.erase(it);
        }
        else if (cache.size() == capacity) {
            auto last = cache.back();
            int lastKey = last.first;
            keyMap.erase(lastKey);
            cache.pop_back();
        }
        
        cache.push_front({key, value});
        keyMap[key] = cache.begin();
    }
};

int main() {
    
    int cap, Q;
    
    // Ввод емкости кэша
    cout << "Введите емкость кэша (cap): ";
    cin >> cap;
    
    // Ввод количества запросов
    cout << "Введите количество запросов (Q): ";
    cin >> Q;
    
    cin.ignore(); // Очищаем буфер
    
    LRUCache cache(cap);
    vector<int> results;
    
    cout << "\nВведите " << Q << " запросов (каждый с новой строки):" << endl;
    cout << "Формат: SET <ключ> <значение> или GET <ключ>" << endl;
    
    for (int i = 0; i < Q; i++) {
        cout << "Запрос " << i + 1 << ": ";
        
        string query;
        getline(cin, query);
        
        stringstream ss(query);
        string command;
        ss >> command;
        
        if (command == "SET") {
            int key, value;
            ss >> key >> value;
            cache.set(key, value);
        } 
        else if (command == "GET") {
            int key;
            ss >> key;
            results.push_back(cache.get(key));
        }
        else {
            cout << "Ошибка: неизвестная команда '" << command << "'" << endl;
            i--; // Повторяем ввод этого запроса
        }
    }
    
    // Вывод результатов
    cout << "\nРезультаты GET запросов:" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << results[i];
        if (i != results.size() - 1) {
            cout << " ";
        }
    }
    cout << endl;
    
    return 0;
}