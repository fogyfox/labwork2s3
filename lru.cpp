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
    //храним пары ключ-значение в двусвязном списке для быстрого перемещения
    list<pair<int, int>> cache;
    //хеш-таблица для быстрого доступа к элементам по ключу
    unordered_map<int, list<pair<int, int>>::iterator> keyMap;

public:
    LRUCache(int cap) {
        capacity = cap;
    }

    int get(int key) {
        //если ключ не найден, возвращаем -1
        if (keyMap.find(key) == keyMap.end()) {
            return -1;
        }
        
        //перемещаем элемент в начало списка (самый недавно использованный)
        auto it = keyMap[key];
        int value = it->second;
        cache.erase(it);
        cache.push_front({key, value});
        keyMap[key] = cache.begin();
        
        return value;
    }

    void set(int key, int value) {
        //если ключ уже существует, обновляем значение и перемещаем в начало
        if (keyMap.find(key) != keyMap.end()) {
            auto it = keyMap[key];
            cache.erase(it);
        }
        //если ключ новый и кэш полон, удаляем самый старый элемент
        else if (cache.size() == capacity) {
            auto last = cache.back();
            int lastKey = last.first;
            keyMap.erase(lastKey);
            cache.pop_back();
        }
        
        //добавляем новый элемент в начало
        cache.push_front({key, value});
        keyMap[key] = cache.begin();
    }
};

//функция для обработки запросов
vector<int> processQueries(int capacity, const vector<string>& queries) {
    LRUCache cache(capacity);
    vector<int> results;
    
    for (const string& query : queries) {
        stringstream ss(query);
        string command;
        ss >> command;
        
        if (command == "SET") {
            int key, value;
            ss >> key >> value;
            cache.set(key, value);
        } else if (command == "GET") {
            int key;
            ss >> key;
            results.push_back(cache.get(key));
        }
    }
    
    return results;
}

int main() {
    cout << "Пример 1:" << endl;
    int cap1 = 2;
    vector<string> queries1 = {"SET 1 2", "GET 1"};
    vector<int> result1 = processQueries(cap1, queries1);
    
    for (int res : result1) {
        cout << res << " ";
    }
    cout << endl;
    
    cout << "Пример 2:" << endl;
    int cap2 = 2;
    vector<string> queries2 = {
        "SET 1 2", "SET 2 3", "SET 1 5", "SET 4 5", 
        "SET 6 7", "GET 4", "SET 1 2", "GET 3"
    };
    vector<int> result2 = processQueries(cap2, queries2);
    
    for (int res : result2) {
        cout << res << " ";
    }
    cout << endl;
    
    return 0;
}