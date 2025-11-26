#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include "set.h"

using namespace std;

//база данных - теперь только множества
map<string, Set*> sets;

//функции для работы с файлом
void saveToFile(const string& filename);
void loadFromFile(const string& filename);

//функции для обработки команд множества
void processSetQuery(const vector<string>& tokens);
void processSetOperation(const vector<string>& tokens);
void processPartitionOperation(const vector<string>& tokens);

//вспомогательные функции
vector<string> split(const string& str, char delimiter);
int stringToInt(const string& str);

int main(int argc, char* argv[]) {
    string filename;
    string query;

    //разбор аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[++i];
        }
    }

    if (filename.empty() || query.empty()) {
        cout << "Использование: " << argv[0] << " --file <filename> --query '<command>'" << endl;
        return 1;
    }

    //загрузка данных из файла
    loadFromFile(filename);

    //разбор и выполнение запроса
    vector<string> tokens = split(query, ' ');
    
    if (tokens.empty()) {
        cout << "Пустой запрос" << endl;
        return 1;
    }

    string command = tokens[0];
    
    try {
        if (command == "SINSERT" || command == "SCONTAINS" || command == "SREMOVE" || 
            command == "SSIZE" || command == "SEMPTY" || command == "SCLEAR") {
            processSetQuery(tokens);
        } else if (command == "SUNION" || command == "SINTERSECTION" || 
                   command == "SDIFFERENCE" || command == "SSUBSET") {
            processSetOperation(tokens);
        } else if (command == "SPRINT") {
            if (tokens.size() < 2) {
                cout << "SPRINT требует имя множества" << endl;
                return 1;
            }
            string setName = tokens[1];
            if (sets.find(setName) != sets.end()) {
                cout << "Множество '" << setName << "': ";
                printSet(sets[setName]);
            } else {
                cout << "Множество '" << setName << "' не найдено" << endl;
            }
        } else if (command == "SPARTITION") {
            processPartitionOperation(tokens);
        } else {
            cout << "Неизвестная команда: " << command << endl;
            return 1;
        }
    } catch (const exception& e) {
        cout << "Ошибка выполнения: " << e.what() << endl;
        return 1;
    }

    //сохранение данных в файл
    saveToFile(filename);

    //очистка памяти
    for (auto& pair : sets) {
        destroySet(pair.second);
        delete pair.second;
    }

    return 0;
}

//функция для разбиения запроса 
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

//преобразование строки в число с обработкой ошибок
int stringToInt(const string& str) {
    try {
        return stoi(str);
    } catch (const exception& e) {
        throw runtime_error("Неверный числовой формат: " + str);
    }
}

//обработка базовых операций с множеством
void processSetQuery(const vector<string>& tokens) {
    if (tokens.size() < 2) {
        cout << "Недостаточно аргументов для команды множества" << endl;
        return;
    }

    string command = tokens[0];
    string setName = tokens[1];

    //создаем множество, если не существует
    if (sets.find(setName) == sets.end()) {
        sets[setName] = new Set();
        createSet(sets[setName]);
    }

    Set* set = sets[setName];

    if (command == "SINSERT") {
        if (tokens.size() < 3) {
            cout << "SINSERT требует значение" << endl;
            return;
        }
        int value = stringToInt(tokens[2]);
        if (insert(set, value)) {
            cout << "OK" << endl;
        } else {
            cout << "Элемент уже существует" << endl;
        }
    }
    else if (command == "SCONTAINS") {
        if (tokens.size() < 3) {
            cout << "SCONTAINS требует значение" << endl;
            return;
        }
        int value = stringToInt(tokens[2]);
        bool exists = contains(set, value);
        cout << (exists ? "TRUE" : "FALSE") << endl;
    }
    else if (command == "SREMOVE") {
        if (tokens.size() < 3) {
            cout << "SREMOVE требует значение" << endl;
            return;
        }
        int value = stringToInt(tokens[2]);
        if (remove(set, value)) {
            cout << "OK" << endl;
        } else {
            cout << "Элемент не найден" << endl;
        }
    }
    else if (command == "SSIZE") {
        cout << size(set) << endl;
    }
    else if (command == "SEMPTY") {
        cout << (empty(set) ? "TRUE" : "FALSE") << endl;
    }
    else if (command == "SCLEAR") {
        clear(set);
        cout << "OK" << endl;
    }
}

//обработка операций между множествами
void processSetOperation(const vector<string>& tokens) {
    if (tokens.size() < 3) {
        cout << "Недостаточно аргументов для операции с множествами" << endl;
        return;
    }

    string command = tokens[0];
    string resultSetName = tokens[1];
    string set1Name = tokens[2];
    string set2Name = (tokens.size() > 3) ? tokens[3] : "";

    //проверяем существование исходных множеств
    if (sets.find(set1Name) == sets.end()) {
        cout << "Множество '" << set1Name << "' не найдено" << endl;
        return;
    }

    Set* set1 = sets[set1Name];
    Set* set2 = nullptr;

    if (!set2Name.empty() && sets.find(set2Name) == sets.end()) {
        cout << "Множество '" << set2Name << "' не найдено" << endl;
        return;
    } else if (!set2Name.empty()) {
        set2 = sets[set2Name];
    }

    Set result;

    if (command == "SUNION") {
        if (!set2) {
            cout << "SUNION требует два множества" << endl;
            return;
        }
        result = unionSets(set1, set2);
    }
    else if (command == "SINTERSECTION") {
        if (!set2) {
            cout << "SINTERSECTION требует два множества" << endl;
            return;
        }
        result = intersection(set1, set2);
    }
    else if (command == "SDIFFERENCE") {
        if (!set2) {
            cout << "SDIFFERENCE требует два множества" << endl;
            return;
        }
        result = difference(set1, set2);
    }
    else if (command == "SSUBSET") {
        if (!set2) {
            cout << "SSUBSET требует два множества" << endl;
            return;
        }
        bool isSub = isSubset(set1, set2);
        cout << (isSub ? "TRUE" : "FALSE") << endl;
        return; //не создаем новое множество для SUBSET
    }

    //сохраняем результат
    if (sets.find(resultSetName) != sets.end()) {
        destroySet(sets[resultSetName]);
        delete sets[resultSetName];
    }
    sets[resultSetName] = new Set(result);
    cout << "OK" << endl;
}

//обработка операции разбиения множества
void processPartitionOperation(const vector<string>& tokens) {
    if (tokens.size() < 4) {
        cout << "Использование: SPARTITION <исходное_множество> <сумма_подмножества> <префикс_результата>" << endl;
        return;
    }

    string sourceSetName = tokens[1];
    int targetSum = stringToInt(tokens[2]);
    string resultPrefix = tokens[3];

    //проверяем существование исходного множества
    if (sets.find(sourceSetName) == sets.end()) {
        cout << "Множество '" << sourceSetName << "' не найдено" << endl;
        return;
    }

    Set* sourceSet = sets[sourceSetName];
    
    if (empty(sourceSet)) {
        cout << "Исходное множество пусто" << endl;
        return;
    }

    vector<vector<int>> partitions;
    
    cout << "Разбиение множества '" << sourceSetName << "' на подмножества с суммой " << targetSum << "..." << endl;
    
    if (partitionSetImproved(sourceSet, targetSum, partitions)) {
        cout << "Успешно создано " << partitions.size() << " подмножеств:" << endl;
        
        //сохраняем результаты как отдельные множества
        for (size_t i = 0; i < partitions.size(); i++) {
            string resultSetName = resultPrefix + "_" + to_string(i + 1);
            
            //удаляем старое множество, если существует
            if (sets.find(resultSetName) != sets.end()) {
                destroySet(sets[resultSetName]);
                delete sets[resultSetName];
            }
            
            //создаем новое множество
            sets[resultSetName] = new Set();
            createSet(sets[resultSetName]);
            
            //заполняем элементами подмножества
            for (int num : partitions[i]) {
                insert(sets[resultSetName], num);
            }
            
            //выводим информацию о подмножестве
            cout << "Подмножество " << i + 1 << " (" << resultSetName << "): {";
            for (size_t j = 0; j < partitions[i].size(); j++) {
                cout << partitions[i][j];
                if (j < partitions[i].size() - 1) cout << ", ";
            }
            cout << "} (сумма = ";
            
            //проверяем сумму
            int actualSum = 0;
            for (int num : partitions[i]) {
                actualSum += num;
            }
            cout << actualSum << ")" << endl;
        }
        cout << "OK" << endl;
    } else {
        cout << "ОШИБКА: Невозможно разбить множество на подмножества с суммой " << targetSum << endl;
    }
}

void saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file) {
        cout << "Ошибка открытия файла для записи: " << filename << endl;
        return;
    }

    //сохранение множеств
    for (const auto& pair : sets) {
        file << "SET " << pair.first << " ";
        
        //сохраняем все элементы множества
        Set* set = pair.second;
        for (int i = 0; i < set->tableSize; i++) {
            NodeSet* current = set->buckets[i];
            while (current != nullptr) {
                file << current->key << " ";
                current = current->next;
            }
        }
        file << endl;
    }

    file.close();
    cout << "Данные сохранены в файл: " << filename << endl;
}

void loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Файл '" << filename << "' не найден. Будет создан новый." << endl;
        return; //файл может не существовать при первом запуске
    }

    string line;
    int loadedCount = 0;
    
    while (getline(file, line)) {
        vector<string> tokens = split(line, ' ');
        if (tokens.size() < 2) continue;

        string type = tokens[0];
        string name = tokens[1];

        if (type == "SET") {
            //удаляем старое множество, если существует
            if (sets.find(name) != sets.end()) {
                destroySet(sets[name]);
                delete sets[name];
            }
            
            sets[name] = new Set();
            createSet(sets[name]);
            
            for (size_t i = 2; i < tokens.size(); i++) {
                try {
                    int value = stringToInt(tokens[i]);
                    insert(sets[name], value);
                } catch (const exception& e) {
                    cout << "Предупреждение: неверный элемент '" << tokens[i] 
                         << "' в множестве '" << name << "' - пропущен" << endl;
                }
            }
            loadedCount++;
        }
    }

    file.close();
    cout << "Загружено " << loadedCount << " множеств из файла: " << filename << endl;
}