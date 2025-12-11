package main

import (
	"bufio"
	"container/list"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// LRUCache - структура LRU кэша
type LRUCache struct {
	capacity int
	cache    *list.List
	keyMap   map[int]*list.Element
}

// cacheItem - элемент кэша
type cacheItem struct {
	key   int
	value int
}

// NewLRUCache - создание нового LRU кэша
func NewLRUCache(capacity int) *LRUCache {
	return &LRUCache{
		capacity: capacity,
		cache:    list.New(),
		keyMap:   make(map[int]*list.Element),
	}
}

// Get - получение значения по ключу
func (lru *LRUCache) Get(key int) int {
	if elem, exists := lru.keyMap[key]; exists {
		// Перемещаем элемент в начало (самый свежий)
		lru.cache.MoveToFront(elem)
		return elem.Value.(*cacheItem).value
	}
	return -1
}

// Set - установка значения по ключу
func (lru *LRUCache) Set(key, value int) {
	if elem, exists := lru.keyMap[key]; exists {
		// Обновляем значение и перемещаем в начало
		elem.Value.(*cacheItem).value = value
		lru.cache.MoveToFront(elem)
		return
	}

	// Если кэш полон, удаляем самый старый элемент
	if lru.cache.Len() == lru.capacity {
		back := lru.cache.Back()
		if back != nil {
			oldKey := back.Value.(*cacheItem).key
			delete(lru.keyMap, oldKey)
			lru.cache.Remove(back)
		}
	}

	// Добавляем новый элемент в начало
	newItem := &cacheItem{key: key, value: value}
	elem := lru.cache.PushFront(newItem)
	lru.keyMap[key] = elem
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	
	// Ввод емкости кэша
	fmt.Print("Введите емкость кэша (cap): ")
	capStr, _ := reader.ReadString('\n')
	capStr = strings.TrimSpace(capStr)
	capacity, _ := strconv.Atoi(capStr)
	
	// Ввод количества запросов
	fmt.Print("Введите количество запросов (Q): ")
	qStr, _ := reader.ReadString('\n')
	qStr = strings.TrimSpace(qStr)
	Q, _ := strconv.Atoi(qStr)
	
	cache := NewLRUCache(capacity)
	var results []int
	
	fmt.Printf("\nВведите %d запросов (каждый с новой строки):\n", Q)
	fmt.Println("Формат: SET <ключ> <значение> или GET <ключ>")
	
	for i := 0; i < Q; i++ {
		fmt.Printf("Запрос %d: ", i+1)
		query, _ := reader.ReadString('\n')
		query = strings.TrimSpace(query)
		
		parts := strings.Fields(query)
		if len(parts) == 0 {
			fmt.Println("Ошибка: пустой запрос")
			i-- // Повторяем ввод
			continue
		}
		
		command := parts[0]
		
		switch strings.ToUpper(command) {
		case "SET":
			if len(parts) != 3 {
				fmt.Println("Ошибка: формат команды SET <ключ> <значение>")
				i-- // Повторяем ввод
				continue
			}
			
			key, err1 := strconv.Atoi(parts[1])
			value, err2 := strconv.Atoi(parts[2])
			
			if err1 != nil || err2 != nil {
				fmt.Println("Ошибка: ключ и значение должны быть целыми числами")
				i-- // Повторяем ввод
				continue
			}
			
			cache.Set(key, value)
			
		case "GET":
			if len(parts) != 2 {
				fmt.Println("Ошибка: формат команды GET <ключ>")
				i-- // Повторяем ввод
				continue
			}
			
			key, err := strconv.Atoi(parts[1])
			if err != nil {
				fmt.Println("Ошибка: ключ должен быть целым числом")
				i-- // Повторяем ввод
				continue
			}
			
			result := cache.Get(key)
			results = append(results, result)
			
		default:
			fmt.Printf("Ошибка: неизвестная команда '%s'\n", command)
			i-- // Повторяем ввод
		}
	}
	
	// Вывод результатов
	fmt.Println("\nРезультаты GET запросов:")
	for i, result := range results {
		fmt.Print(result)
		if i != len(results)-1 {
			fmt.Print(" ")
		}
	}
	fmt.Println()
}