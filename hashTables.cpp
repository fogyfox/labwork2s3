#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <stdexcept>
#include <string>

//базовый класс для хеш-таблицы
template<typename K, typename V>
class HashTable {
protected:
    size_t capacity;
    size_t size;
    double loadFactorThreshold;

    virtual void rehash() = 0;

public:
    HashTable(size_t initialCapacity = 16, double loadFactor = 0.9) 
        : capacity(initialCapacity), size(0), loadFactorThreshold(loadFactor) {}

    virtual ~HashTable() = default;

    virtual void insert(const K& key, const V& value) = 0;
    virtual bool find(const K& key, V& value) const = 0;
    virtual bool remove(const K& key) = 0;
    virtual void display() const = 0;

    double loadFactor() const {
        return static_cast<double>(size) / capacity;
    }

    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }
};

//хеш-таблица с методом цепочек
template<typename K, typename V>
class ChainingHashTable : public HashTable<K, V> {
private:
    std::vector<std::list<std::pair<K, V>>> table;

    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % this->capacity;
    }

    void rehash() override {
        size_t newCapacity = this->capacity * 2;
        std::vector<std::list<std::pair<K, V>>> newTable(newCapacity);

        //перехеширование всех элементов
        for (const auto& chain : table) {
            for (const auto& pair : chain) {
                size_t newIndex = std::hash<K>{}(pair.first) % newCapacity;
                newTable[newIndex].push_back(pair);
            }
        }

        table = std::move(newTable);
        this->capacity = newCapacity;
        std::cout << "Rehashed to capacity: " << this->capacity << std::endl;
    }

public:
    ChainingHashTable(size_t initialCapacity = 16, double loadFactor = 0.9) 
        : HashTable<K, V>(initialCapacity, loadFactor), table(initialCapacity) {}

    void insert(const K& key, const V& value) override {
        //проверка необходимости рехеширования
        if (this->loadFactor() >= this->loadFactorThreshold) {
            rehash();
        }

        size_t index = hash(key);
        
        //проверка на существование ключа
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value; // Обновление значения
                return;
            }
        }

        //вставка нового элемента
        table[index].emplace_back(key, value);
        this->size++;
    }

    bool find(const K& key, V& value) const override {
        size_t index = hash(key);
        
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        return false;
    }

    bool remove(const K& key) override {
        size_t index = hash(key);
        
        auto& chain = table[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == key) {
                chain.erase(it);
                this->size--;
                return true;
            }
        }
        return false;
    }

    void display() const override {
        std::cout << "Chaining Hash Table (size: " << this->size 
                  << ", capacity: " << this->capacity 
                  << ", load factor: " << this->loadFactor() << ")" << std::endl;
        
        for (size_t i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) {
                std::cout << "Bucket " << i << ": ";
                for (const auto& pair : table[i]) {
                    std::cout << "[" << pair.first << ":" << pair.second << "] ";
                }
                std::cout << std::endl;
            }
        }
    }
};

//хеш-таблица с открытой адресацией
template<typename K, typename V>
class OpenAddressingHashTable : public HashTable<K, V> {
private:
    enum class EntryState { EMPTY, OCCUPIED, DELETED };
    
    struct Entry {
        K key;
        V value;
        EntryState state;
        
        Entry() : state(EntryState::EMPTY) {}
    };

    std::vector<Entry> table;

    size_t hash(const K& key, size_t attempt) const {
        //двойное хеширование
        size_t h1 = std::hash<K>{}(key) % this->capacity;
        size_t h2 = 1 + (std::hash<K>{}(key) % (this->capacity - 1));
        return (h1 + attempt * h2) % this->capacity;
    }

    void rehash() override {
        size_t newCapacity = this->capacity * 2;
        std::vector<Entry> newTable(newCapacity);

        //перехеширование всех элементов
        for (const auto& entry : table) {
            if (entry.state == EntryState::OCCUPIED) {
                for (size_t attempt = 0; attempt < newCapacity; ++attempt) {
                    size_t index = std::hash<K>{}(entry.key) % newCapacity;
                    //линейное пробирование для простоты при рехешировании
                    size_t newIndex = (index + attempt) % newCapacity;
                    
                    if (newTable[newIndex].state == EntryState::EMPTY) {
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
        std::cout << "Rehashed to capacity: " << this->capacity << std::endl;
    }

public:
    OpenAddressingHashTable(size_t initialCapacity = 16, double loadFactor = 0.9) 
        : HashTable<K, V>(initialCapacity, loadFactor), table(initialCapacity) {}

    void insert(const K& key, const V& value) override {
        //проверка необходимости рехеширования
        if (this->loadFactor() >= this->loadFactorThreshold) {
            rehash();
        }

        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY || 
                table[index].state == EntryState::DELETED) {
                //вставка нового элемента
                table[index].key = key;
                table[index].value = value;
                table[index].state = EntryState::OCCUPIED;
                this->size++;
                return;
            } else if (table[index].state == EntryState::OCCUPIED && 
                       table[index].key == key) {
                //обновление существующего элемента
                table[index].value = value;
                return;
            }
        }

        throw std::runtime_error("Hash table is full");
    }

    bool find(const K& key, V& value) const override {
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false; //элемент не найден
            } else if (table[index].state == EntryState::OCCUPIED && 
                       table[index].key == key) {
                value = table[index].value;
                return true;
            }
        }
        return false;
    }

    bool remove(const K& key) override {
        for (size_t attempt = 0; attempt < this->capacity; ++attempt) {
            size_t index = hash(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false; //элемент не найден
            } else if (table[index].state == EntryState::OCCUPIED && 
                       table[index].key == key) {
                table[index].state = EntryState::DELETED;
                this->size--;
                return true;
            }
        }
        return false;
    }

    void display() const override {
        std::cout << "Open Addressing Hash Table (size: " << this->size 
                  << ", capacity: " << this->capacity 
                  << ", load factor: " << this->loadFactor() << ")" << std::endl;
        
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].state == EntryState::OCCUPIED) {
                std::cout << "Index " << i << ": [" << table[i].key 
                          << ":" << table[i].value << "]" << std::endl;
            } else if (table[i].state == EntryState::DELETED) {
                std::cout << "Index " << i << ": [DELETED]" << std::endl;
            }
        }
    }
};

//демонстрация работы
//демонстрация работы хеш-таблиц
int main() {
    std::cout << "=== Демонстрация хеш-таблицы с методом цепочек ===" << std::endl;
    ChainingHashTable<std::string, int> chTable(5, 0.9); //начальный размер 5, порог 90%
    
    //вставляем элементы - должно произойти рехеширование
    std::cout << "\n1. Вставка 8 элементов в таблицу размером 5:" << std::endl;
    for (int i = 1; i <= 8; ++i) {
        std::string ключ = "ключ" + std::to_string(i);
        chTable.insert(ключ, i * 100);
        std::cout << "   Добавлен: " << ключ << " -> " << i * 100 << std::endl;
    }
    
    chTable.display();
    
    //поиск существующих элементов
    std::cout << "\n2. Поиск элементов:" << std::endl;
    int value;
    if (chTable.find("ключ3", value)) {
        std::cout << "   Найден ключ3: " << value << std::endl;
    }
    
    if (chTable.find("ключ7", value)) {
        std::cout << "   Найден ключ7: " << value << std::endl;
    }
    
    //попытка найти несуществующий ключ
    if (!chTable.find("несуществующий", value)) {
        std::cout << "   Ключ 'несуществующий' не найден" << std::endl;
    }
    
    //удаление элемента
    std::cout << "\n3. Удаление элемента 'ключ4':" << std::endl;
    if (chTable.remove("ключ4")) {
        std::cout << "   Ключ4 успешно удален" << std::endl;
    }
    chTable.display();
    
    //обновление значения
    std::cout << "\n4. Обновление значения ключ2:" << std::endl;
    chTable.insert("ключ2", 9999);
    if (chTable.find("ключ2", value)) {
        std::cout << "   Обновленное значение ключ2: " << value << std::endl;
    }
    
    std::cout << "\n=== Демонстрация хеш-таблицы с открытой адресацией ===" << std::endl;
    OpenAddressingHashTable<std::string, std::string> adTable(4, 0.9);
    
    //вставляем строковые значения
    std::cout << "\n1. Вставка элементов:" << std::endl;
    adTable.insert("имя", "Иван");
    adTable.insert("фамилия", "Петров");
    adTable.insert("город", "Москва");
    adTable.insert("страна", "Россия");
    
    //этот элемент вызовет рехеширование
    adTable.insert("профессия", "Программист");
    
    adTable.display();
    
    //поиск элементов
    std::cout << "\n2. Поиск элементов:" << std::endl;
    std::string strValue;
    if (adTable.find("город", strValue)) {
        std::cout << "   Город: " << strValue << std::endl;
    }
    
    if (adTable.find("профессия", strValue)) {
        std::cout << "   Профессия: " << strValue << std::endl;
    }
    
    //удаление и повторная вставка
    std::cout << "\n3. Удаление и повторная вставка:" << std::endl;
    adTable.remove("страна");
    std::cout << "   Удален элемент 'страна'" << std::endl;
    
    adTable.insert("страна", "Беларусь");
    std::cout << "   Добавлен элемент 'страна' с новым значением" << std::endl;
    
    if (adTable.find("страна", strValue)) {
        std::cout << "   Новая страна: " << strValue << std::endl;
    }
    
    adTable.display();
    
    //демонстрация коэффициента заполнения
    std::cout << "\n=== Статистика ===" << std::endl;
    std::cout << "Таблица цепочек - размер: " << chTable.getSize() 
              << ", вместимость: " << chTable.getCapacity()
              << ", коэффициент заполнения: " << chTable.loadFactor() << std::endl;
    
    std::cout << "Таблица адресации - размер: " << adTable.getSize() 
              << ", вместимость: " << adTable.getCapacity()
              << ", коэффициент заполнения: " << adTable.loadFactor() << std::endl;
    
    return 0;
}