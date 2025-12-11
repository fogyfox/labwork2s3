package main

import (
	"fmt"
	"time"
)


// Pair - пара ключ-значение
type Pair struct {
	key   string
	value interface{}
}

// SimpleChainingHashTable - упрощенная хеш-таблица с цепочками
type SimpleChainingHashTable struct {
	capacity   int
	size       int
	loadFactor float64
	table      [][]Pair
}

// NewSimpleChainingHashTable - создание новой таблицы
func NewSimpleChainingHashTable(initialCapacity int, loadFactor float64) *SimpleChainingHashTable {
	if initialCapacity <= 0 {
		initialCapacity = 16
	}
	
	return &SimpleChainingHashTable{
		capacity:   initialCapacity,
		size:       0,
		loadFactor: loadFactor,
		table:      make([][]Pair, initialCapacity),
	}
}

// simpleHash - простая хеш-функция для строк
func simpleHash(key string, capacity int) int {
	hash := 0
	for i := 0; i < len(key); i++ {
		hash = (hash*31 + int(key[i])) % capacity
	}
	return hash
}

// Insert - вставка элемента
func (ht *SimpleChainingHashTable) Insert(key string, value interface{}) {
	// Проверка на рехеширование
	if float64(ht.size)/float64(ht.capacity) >= ht.loadFactor {
		ht.rehash()
	}
	
	index := simpleHash(key, ht.capacity)
	
	// Ищем существующий ключ
	for i, p := range ht.table[index] {
		if p.key == key {
			// Обновляем значение
			ht.table[index][i].value = value
			return
		}
	}
	
	// Добавляем новую пару
	ht.table[index] = append(ht.table[index], Pair{key, value})
	ht.size++
}

// rehash - рехеширование
func (ht *SimpleChainingHashTable) rehash() {
	newCapacity := ht.capacity * 2
	newTable := make([][]Pair, newCapacity)
	
	// Перехешируем все элементы
	for _, chain := range ht.table {
		for _, p := range chain {
			newIndex := simpleHash(p.key, newCapacity)
			newTable[newIndex] = append(newTable[newIndex], p)
		}
	}
	
	ht.table = newTable
	ht.capacity = newCapacity
}

// Find - поиск элемента
func (ht *SimpleChainingHashTable) Find(key string) (interface{}, bool) {
	index := simpleHash(key, ht.capacity)
	
	for _, p := range ht.table[index] {
		if p.key == key {
			return p.value, true
		}
	}
	
	return nil, false
}

// Remove - удаление элемента
func (ht *SimpleChainingHashTable) Remove(key string) bool {
	index := simpleHash(key, ht.capacity)
	
	for i, p := range ht.table[index] {
		if p.key == key {
			// Удаляем элемент из цепочки
			ht.table[index] = append(ht.table[index][:i], ht.table[index][i+1:]...)
			ht.size--
			return true
		}
	}
	
	return false
}

// Display - вывод содержимого таблицы
func (ht *SimpleChainingHashTable) Display() {
	fmt.Println()
	fmt.Println("ХЕШ-ТАБЛИЦА С МЕТОДОМ ЦЕПОЧЕК (упрощенная)")
	fmt.Printf("Размер: %d, Вместимость: %d\n", ht.size, ht.capacity)
	
	for i, chain := range ht.table {
		if len(chain) > 0 {
			fmt.Printf("Ячейка [%d]: ", i)
			for j, p := range chain {
				fmt.Printf("{%s: %v}", p.key, p.value)
				if j < len(chain)-1 {
					fmt.Print(" ")
				}
			}
			fmt.Println()
		}
	}
}

func main() {
	// Демонстрация упрощенной версии
	table := NewSimpleChainingHashTable(5, 0.8)
	
	// Вставка элементов
	fmt.Println("\n1. Вставка элементов:")
	table.Insert("яблоко", 100)
	table.Insert("банан", 50)
	table.Insert("апельсин", 75)
	table.Insert("груша", 120)
	table.Insert("виноград", 200)
	table.Display()
	
	// Поиск
	fmt.Println("\n2. Поиск элементов:")
	if val, found := table.Find("банан"); found {
		fmt.Printf("   Найден банан: %v\n", val)
	}
	
	if _, found := table.Find("киви"); !found {
		fmt.Println("   Киви не найден (корректно)")
	}
	
	// Удаление
	fmt.Println("\n3. Удаление элемента:")
	table.Remove("апельсин")
	table.Display()
	
	// Обновление
	fmt.Println("\n4. Обновление значения:")
	table.Insert("банан", 60)
	if val, found := table.Find("банан"); found {
		fmt.Printf("   Обновленный банан: %v\n", val)
	}
	
	// Рехеширование
	fmt.Println("\n5. Тест рехеширования (добавляем больше элементов):")
	for i := 0; i < 10; i++ {
		key := fmt.Sprintf("ключ%d", i)
		table.Insert(key, i*10)
	}
	table.Display()
	
	// Измерение времени
	fmt.Println("\n6. Измерение производительности:")
	start := time.Now()
	
	for i := 0; i < 10000; i++ {
		key := fmt.Sprintf("тест_%d", i)
		table.Insert(key, i)
	}
	
	elapsed := time.Since(start)
	fmt.Printf("   Время вставки 10000 элементов: %v\n", elapsed)
	
	// Поиск всех элементов
	searchStart := time.Now()
	for i := 0; i < 10000; i++ {
		key := fmt.Sprintf("тест_%d", i)
		table.Find(key)
	}
	searchElapsed := time.Since(searchStart)
	fmt.Printf("   Время поиска 10000 элементов: %v\n", searchElapsed)
}