#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <chrono>   
#include <random>    
#include <algorithm> 

using namespace std;
using namespace std::chrono;

//Базовый класс для хеш-таблицы
template<typename K, typename V>
class HashTable {
protected:
    size_t capacity; 
    size_t size;
    double loadFactorThreshold; // Пороговое значение коэффициента заполнения

    virtual void rehash() = 0;

public:
    HashTable(size_t initialCapacity = 16, double loadFactor = 0.9)
        : capacity(initialCapacity), size(0), loadFactorThreshold(loadFactor) {}

    virtual ~HashTable() = default;

    // Основные операции (чисто виртуальные функции)
    virtual void insert(const K& key, const V& value) = 0; // Вставка
    virtual bool find(const K& key, V& value) const = 0; // Поиск
    virtual bool remove(const K& key) = 0; // Удаление
    virtual void display() const = 0; // Вывод содержимого

    // Метод для расчета текущего коэффициента заполнения
    double loadFactor() const {
        return static_cast<double>(size) / capacity;
    }

    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }
    
    // Измерения времени поиска
    virtual double measureFindTime(const vector<K>& keysToSearch, int m) const {
        V dummy;
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < m; i++) {
            for (const auto& key : keysToSearch) {
                find(key, dummy);
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        return duration.count() / 1000000.0; // Возвращаем время в секундах
    }
};


// Хеш-таблица с методом цепочек
template<typename K, typename V>
class ChainingHashTable : public HashTable<K, V> {
private:
    std::vector<std::list<std::pair<K, V>>> table;

    // Хеш-функция (вычисляет индекс по ключу)
    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % this->capacity;
    }

    // Рехеширование
    void rehash() override {
        size_t newCapacity = this->capacity * 2;  // Удваиваем вместимость
        std::vector<std::list<std::pair<K, V>>> newTable(newCapacity);

        for (const auto& chain : table) {
            for (const auto& pair : chain) {
                // Вычисляем новый индекс для каждого элемента
                size_t newIndex = std::hash<K>{}(pair.first) % newCapacity;
                newTable[newIndex].push_back(pair);
            }
        }

        // Заменяем старую таблицу новой
        table = std::move(newTable);
        this->capacity = newCapacity;
    }

public:
    ChainingHashTable(size_t initialCapacity = 16, double loadFactor = 0.9)
        : HashTable<K, V>(initialCapacity, loadFactor), table(initialCapacity) {}

    void insert(const K& key, const V& value) override {
        // Проверка на рехеширование
        if (this->loadFactor() >= this->loadFactorThreshold) {
            rehash();
        }

        size_t index = hash(key);  // Вычисляем индекс (номер корзины)

        // Ищем заданный ключ
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value;// Обновляем значение, если ключ найден
                return;
            }
        }

        table[index].emplace_back(key, value);// Добавляем новую пару в конец цепочки
        this->size++;
    }

    // Поиск элемента по ключу
    bool find(const K& key, V& value) const override {
        size_t index = hash(key);
        
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;// Записываем найденное значение
                return true;
            }
        }
        return false;
    }

    // Удаление элемента по ключу
    bool remove(const K& key) override {
        size_t index = hash(key);
        
        auto& chain = table[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == key) {
                chain.erase(it);  // Удаляем элемент
                this->size--;
                return true;
            }
        }
        return false;
    }

    // Вывод содержимого таблицы на экран
    void display() const override {
        std::cout << std::endl;
        std::cout << "ХЕШ-ТАБЛИЦА С МЕТОДОМ ЦЕПОЧЕК\n";
        std::cout << "Размер: " << this->size
                  << ", Вместимость: " << this->capacity
                  << ", Коэффициент заполнения: " << this->loadFactor() << "\n";

        bool isEmpty = true;
        // Выводим только непустые яйчейки
        for (size_t i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) {
                isEmpty = false;
                std::cout << "Яйчейка [" << i << "]: ";
                for (const auto& pair : table[i]) {
                    std::cout << "{" << pair.first << " → " << pair.second << "} ";
                }
                std::cout << std::endl;
            }
        }

        if (isEmpty) {
            std::cout << "(Таблица пуста)\n";
        }
    }
};

// Хеш-таблица с открытой адресацией
template<typename K, typename V>
class OpenAddressingHashTable : public HashTable<K, V> {
private:
    enum class EntryState{ EMPTY,    // Пустая
                           OCCUPIED,  // Занятая
                           DELETED }; // Удаленная

    // Структура для хранения элемента таблицы
    struct Entry {
        K key;
        V value;
        EntryState state; // Состояние ячейки

        Entry() : state(EntryState::EMPTY) {}// По умолчанию ячейка пуста
    };

    std::vector<Entry> table;

    // Хеш-функция с двойным хешированием для разрешения коллизий
    size_t hash(const K& key, size_t attempt) const {
        size_t h1 = std::hash<K>{}(key) % this->capacity;// Первая хеш-функция
        size_t h2 = 1 + (std::hash<K>{}(key) % (this->capacity - 1));// Вторая хеш-функция (никогда не должна возвращать 0)
        return (h1 + attempt * h2) % this->capacity; // Слияние
    }

    // Рехеширование
    void rehash() override {
        size_t newCapacity = this->capacity * 2;
        std::vector<Entry> newTable(newCapacity);

        // Перехеширование всех занятых ячеек
        for (const auto& entry : table) {
            if (entry.state == EntryState::OCCUPIED) {
                for (size_t attempt = 0; attempt < newCapacity; ++attempt) {
                    size_t index = std::hash<K>{}(entry.key) % newCapacity;
                    size_t newIndex = (index + attempt) % newCapacity;
                    
                    if (newTable[newIndex].state == EntryState::EMPTY) {// Нашли пустую ячейку - перемещаем элемент
                        newTable[newIndex].key = entry.key;
                        newTable[newIndex].value = entry.value;
                        newTable[newIndex].state = EntryState::OCCUPIED;
                        break;
                    }
                }
            }
        }

        table = std::move(newTable);
        this->capacity = newCapacity;
    }

public:
    OpenAddressingHashTable(size_t initialCapacity = 16, double loadFactor = 0.9)
        : HashTable<K, V>(initialCapacity, loadFactor), table(initialCapacity) {}

    // Вставка элемента
    void insert(const K& key, const V& value) override {
        // Проверяем необходимость рехеширования
        if (this->loadFactor() >= this->loadFactorThreshold) {
            rehash();
        }

        // Ищем существующий ключ для обновления
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::OCCUPIED && 
                table[index].key == key) {
 
                table[index].value = value;// Ключ уже существует - обновляем значение
                return;
            }
        }

        // Ищем пустую или удаленную ячейку для вставки нового ключа
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY || 
                table[index].state == EntryState::DELETED) {
                table[index].key = key;
                table[index].value = value;
                table[index].state = EntryState::OCCUPIED;
                this->size++;
                return;
            }
        }

        // Если все ячейки заняты, выполняем рехеширование и пробуем снова
        rehash();
        
        // Пытаемся вставить после рехеширования
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY || 
                table[index].state == EntryState::DELETED) {
                table[index].key = key;
                table[index].value = value;
                table[index].state = EntryState::OCCUPIED;
                this->size++;
                return;
            }
        }

        // Если даже после рехеширования не нашли места, таблица переполнена
        throw std::runtime_error("Хеш-таблица переполнена после рехеширования");
    }

    // Поиск элемента
    bool find(const K& key, V& value) const override {
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false;
            } else if (table[index].state == EntryState::OCCUPIED && 
                       table[index].key == key) {
                value = table[index].value;
                return true;
            }
        }
        return false;
    }

    // Удаление элемента (логическое удаление)
    bool remove(const K& key) override {
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false;
            } else if (table[index].state == EntryState::OCCUPIED && 
                       table[index].key == key) {
                // Логическое удаление - помечаем ячейку как удаленную
                table[index].state = EntryState::DELETED;
                this->size--;
                return true;
            }
        }
        return false;
    }

    // Вывод содержимого таблицы
    void display() const override {
        std::cout << std::endl;
        std::cout << "ХЕШ-ТАБЛИЦА С ОТКРЫТОЙ АДРЕСАЦИЕЙ\n";
        std::cout << "Размер: " << this->size
                  << ", Вместимость: " << this->capacity
                  << ", Коэффициент заполнения: " << this->loadFactor() << "\n";

        bool hasEntries = false;
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].state == EntryState::OCCUPIED) {
                hasEntries = true;
                std::cout << "Ячейка [" << i << "]: {"
                          << table[i].key << " → " << table[i].value << "}\n";
            } else if (table[i].state == EntryState::DELETED) {
                hasEntries = true;
                std::cout << "Ячейка [" << i << "]: [УДАЛЕНА]\n";
            }
        }

        if (!hasEntries) {
            std::cout << "(Таблица пуста)\n";
        }
    }
};

// Функция для проведения эмпирического анализа
void performEmpiricalAnalysis() {
    cout << endl;
    cout << "ЭМПИРИЧЕСКИЙ АНАЛИЗ ПРОИЗВОДИТЕЛЬНОСТИ ХЕШ-ТАБЛИЦ\n";
    
    // Параметры тестирования - уменьшим размеры для предотвращения ошибок
    vector<int> testSizes = {100, 500, 1000, 2000}; 
    int searchMultiplier = 5;
    
    // Генератор случайных чисел
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> keyDist(0, 1000000);
    uniform_int_distribution<> valueDist(0, 1000);
    
    cout << "┌─────────┬──────────┬──────────────┬──────────────┬──────────────┐\n";
    cout << "│ N (эл.) │ M (поис.)│ Цепочки (сек)│Откр.адр.(сек)│ Ускорение    │\n";
    cout << "├─────────┼──────────┼──────────────┼──────────────┼──────────────┤\n";
    
    for (int n : testSizes) {
        int m = n * searchMultiplier;
        
        try {
            // Создаем тестовые данные
            vector<string> keysToInsert;
            vector<int> valuesToInsert;
            
            // Генерируем уникальные ключи
            for (int i = 0; i < n; i++) {
                string key = "key_" + to_string(keyDist(gen)) + "_" + to_string(i);
                keysToInsert.push_back(key);
                valuesToInsert.push_back(valueDist(gen));
            }
            
            // Создаем вектор ключей для поиска
            vector<string> keysToSearch;
            for (int i = 0; i < m; i++) {
                // 70% существующих ключей, 30% случайных
                if (i % 10 < 7 && keysToInsert.size() > 0) {
                    int idx = i % keysToInsert.size();
                    keysToSearch.push_back(keysToInsert[idx]);
                } else {
                    keysToSearch.push_back("search_key_" + to_string(keyDist(gen)));
                }
            }
            
            // Перемешиваем ключи для поиска
            shuffle(keysToSearch.begin(), keysToSearch.end(), gen);
            
            // Тестируем хеш-таблицу с цепочками
            ChainingHashTable<string, int> chainingTable;
            
            // Вставляем N элементов
            for (int i = 0; i < n; i++) {
                chainingTable.insert(keysToInsert[i], valuesToInsert[i]);
            }
            
            // Измеряем время M поисков
            double chainingTime = chainingTable.measureFindTime(keysToSearch, 1);
            
            // Тестируем хеш-таблицу с открытой адресацией
            OpenAddressingHashTable<string, int> openAddrTable;
            
            // Вставляем N элементов
            for (int i = 0; i < n; i++) {
                openAddrTable.insert(keysToInsert[i], valuesToInsert[i]);
            }
            
            // Измеряем время M поисков
            double openAddrTime = openAddrTable.measureFindTime(keysToSearch, 1);
            
            // Рассчитываем ускорение
            double speedup = 0;
            string speedupStr = "N/A";
            
            if (openAddrTime > 0 && chainingTime > 0) {
                speedup = chainingTime / openAddrTime;
                if (speedup > 1) {
                    speedupStr = "цепоч. x" + to_string(speedup).substr(0, 4);
                } else if (speedup > 0) {
                    speedupStr = "адрес. x" + to_string(1.0/speedup).substr(0, 4);
                }
            }
            
            cout << "│ " << setw(7) << n << " │ " 
                 << setw(8) << m << " │ "
                 << fixed << setprecision(6) << setw(12) << chainingTime << " │ "
                 << setw(12) << openAddrTime << " │ "
                 << setw(12) << speedupStr << " │\n";
                
        } catch (const exception& e) {
            cout << "│ " << setw(7) << n << " │ " 
                 << setw(8) << m << " │ "
                 << setw(12) << "ОШИБКА" << " │ "
                 << setw(12) << e.what() << " │ "
                 << setw(12) << "N/A" << " │\n";
        }
    }
    
    cout << "└─────────┴──────────┴──────────────┴──────────────┴──────────────┘\n";
}

// Функции для демонстрации работы

// Демонстрация работы хеш-таблицы с цепочками
void demonstrateChainingHashTable() {
    std::cout << std::endl;
    std::cout << "ДЕМОНСТРАЦИЯ: ХЕШ-ТАБЛИЦА С МЕТОДОМ ЦЕПОЧЕК\n";

    // Создаем таблицу с маленькой вместимостью, чтобы сработало рехеширование
    ChainingHashTable<std::string, int> table(5, 0.9);

    std::cout << "1. ВСТАВКА 8 ЭЛЕМЕНТОВ:\n";

    for (int i = 1; i <= 8; ++i) {
        std::string key = "ключ" + std::to_string(i);
        int value = i * 100;
        table.insert(key, value);
        std::cout << "   Добавлено: {" << key << " → " << value << "}\n";
    }

    table.display();

    std::cout << "\n2. ОПЕРАЦИИ ПОИСКА:\n";

    int value;
    
    // Поиск существующего элемента
    if (table.find("ключ3", value)) {
        std::cout << "    Найден 'ключ3': " << value << "\n";
    } else {
        std::cout << "    'ключ3' не найден\n";
    }

    // Попытка найти несуществующий элемент
    if (!table.find("несуществующий", value)) {
        std::cout << "    Корректно не найден 'несуществующий'\n";
    }

    std::cout << "\n3. УДАЛЕНИЕ ЭЛЕМЕНТА:\n";

    if (table.remove("ключ4")) {
        std::cout << "    Успешно удален 'ключ4'\n";
    }
    table.display();

    std::cout << "\n4. ОБНОВЛЕНИЕ ЗНАЧЕНИЯ:\n";

    table.insert("ключ2", 9999);
    if (table.find("ключ2", value)) {
        std::cout << "    Обновленное значение 'ключ2': " << value << "\n";
    }
}

// Демонстрация работы хеш-таблицы с открытой адресацией
void demonstrateOpenAddressingHashTable() {
    std::cout << std::endl;
    std::cout << "\nДЕМОНСТРАЦИЯ: ХЕШ-ТАБЛИЦА С ОТКРЫТОЙ АДРЕСАЦИЕЙ\n";
    OpenAddressingHashTable<std::string, std::string> table(4, 0.9);

    std::cout << "1. ВСТАВКА ЭЛЕМЕНТОВ:\n";

    
    // Создаем список элементов для вставки
    std::vector<std::pair<std::string, std::string>> entries = {
        {"имя", "Иван"},
        {"фамилия", "Петров"},
        {"город", "Москва"},
        {"страна", "Россия"},
        {"профессия", "Программист"}  // Этот элемент вызовет рехеширование
    };

    for (const auto& entry : entries) {
        table.insert(entry.first, entry.second);
        std::cout << "   Добавлено: {" << entry.first << " → " << entry.second << "}\n";
    }

    table.display();
    std::cout << std::endl;
    std::cout << "\n2. ОПЕРАЦИИ ПОИСКА:\n";

    std::string value;
    
    if (table.find("город", value)) {
        std::cout << "    Город: " << value << "\n";
    }

    if (table.find("профессия", value)) {
        std::cout << "    Профессия: " << value << "\n";
    }

    std::cout << "\n3. УДАЛЕНИЕ И ПОВТОРНАЯ ВСТАВКА:\n";

    if (table.remove("страна")) {
        std::cout << "    Удален элемент 'страна'\n";
    }
    
    table.insert("страна", "Беларусь");
    std::cout << "    Добавлен элемент 'страна' с новым значением\n";

    if (table.find("страна", value)) {
        std::cout << "    Новая страна: " << value << "\n";
    }

    table.display();
}

int main() {
    demonstrateChainingHashTable();
    demonstrateOpenAddressingHashTable();

    ChainingHashTable<std::string, int> chTable;
    OpenAddressingHashTable<std::string, std::string> adTable;

    // Добавляем тестовые данные для статистики
    chTable.insert("тест1", 100);
    chTable.insert("тест2", 200);
    
    adTable.insert("поле1", "значение1");
    adTable.insert("поле2", "значение2");
    
    // Проводим эмпирический анализ
    performEmpiricalAnalysis();

    return 0;
}