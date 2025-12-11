package main

import (
	"bufio"
	"fmt"
	"hash/fnv"
	"os"
	"sort"
	"strconv"
)

// NodeSet - узел хеш-таблицы
type NodeSet struct {
	key  int
	next *NodeSet
}

// Set - структура множества на основе хеш-таблицы
type Set struct {
	buckets   []*NodeSet
	tableSize int
	itemCount int
}

// NewSet - создание нового множества
func NewSet(initialSize int) *Set {
	if initialSize <= 0 {
		initialSize = 101 // простое число по умолчанию
	}

	buckets := make([]*NodeSet, initialSize)
	return &Set{
		buckets:   buckets,
		tableSize: initialSize,
		itemCount: 0,
	}
}

// DestroySet - уничтожение множества (освобождение памяти)
func DestroySet(set *Set) {
	if set != nil {
		set.Clear()
		set.buckets = nil
		set.tableSize = 0
		set.itemCount = 0
	}
}

// hashFunction - хеш-функция для целых чисел
func hashFunction(key, tableSize int) int {
	h := fnv.New32a()
	h.Write([]byte(strconv.Itoa(key)))
	return int(h.Sum32()) % tableSize
}

// rehashIfNeeded - проверка на необходимость рехеширования
func (set *Set) rehashIfNeeded() {
	loadFactor := float64(set.itemCount) / float64(set.tableSize)
	if loadFactor > 0.7 {
		newSize := set.tableSize * 2
		newBuckets := make([]*NodeSet, newSize)

		// Перехеширование всех элементов
		for i := 0; i < set.tableSize; i++ {
			current := set.buckets[i]
			for current != nil {
				next := current.next
				newIndex := hashFunction(current.key, newSize)
				current.next = newBuckets[newIndex]
				newBuckets[newIndex] = current
				current = next
			}
		}

		set.buckets = newBuckets
		set.tableSize = newSize
	}
}

// Insert - добавление элемента
func (set *Set) Insert(key int) bool {
	if set.Contains(key) {
		return false // элемент уже существует
	}

	index := hashFunction(key, set.tableSize)
	newNode := &NodeSet{
		key:  key,
		next: set.buckets[index],
	}
	set.buckets[index] = newNode
	set.itemCount++

	set.rehashIfNeeded()
	return true
}

// Contains - проверка наличия элемента
func (set *Set) Contains(key int) bool {
	index := hashFunction(key, set.tableSize)
	current := set.buckets[index]

	for current != nil {
		if current.key == key {
			return true
		}
		current = current.next
	}
	return false
}

// Remove - удаление элемента
func (set *Set) Remove(key int) bool {
	index := hashFunction(key, set.tableSize)
	current := set.buckets[index]
	var prev *NodeSet

	for current != nil {
		if current.key == key {
			if prev == nil {
				set.buckets[index] = current.next
			} else {
				prev.next = current.next
			}
			set.itemCount--
			return true
		}
		prev = current
		current = current.next
	}
	return false
}

// Size - получение размера множества
func (set *Set) Size() int {
	return set.itemCount
}

// Empty - проверка пустоты множества
func (set *Set) Empty() bool {
	return set.itemCount == 0
}

// Clear - очистка множества
func (set *Set) Clear() {
	for i := 0; i < set.tableSize; i++ {
		set.buckets[i] = nil
	}
	set.itemCount = 0
}

// ToSlice - преобразование множества в срез
func (set *Set) ToSlice() []int {
	result := make([]int, 0, set.itemCount)
	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			result = append(result, current.key)
			current = current.next
		}
	}
	return result
}

// Union - объединение множеств
func (set *Set) Union(other *Set) *Set {
	result := NewSet(set.tableSize)

	// Добавляем все элементы из первого множества
	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			result.Insert(current.key)
			current = current.next
		}
	}

	// Добавляем все элементы из второго множества
	for i := 0; i < other.tableSize; i++ {
		current := other.buckets[i]
		for current != nil {
			result.Insert(current.key)
			current = current.next
		}
	}

	return result
}

// Intersection - пересечение множеств
func (set *Set) Intersection(other *Set) *Set {
	result := NewSet(set.tableSize)

	// Проходим по меньшему множеству для оптимизации
	var smaller, larger *Set
	if set.itemCount < other.itemCount {
		smaller, larger = set, other
	} else {
		smaller, larger = other, set
	}

	for i := 0; i < smaller.tableSize; i++ {
		current := smaller.buckets[i]
		for current != nil {
			if larger.Contains(current.key) {
				result.Insert(current.key)
			}
			current = current.next
		}
	}

	return result
}

// Difference - разность множеств
func (set *Set) Difference(other *Set) *Set {
	result := NewSet(set.tableSize)

	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			if !other.Contains(current.key) {
				result.Insert(current.key)
			}
			current = current.next
		}
	}

	return result
}

// IsSubset - проверка подмножества
func (set *Set) IsSubset(other *Set) bool {
	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			if !other.Contains(current.key) {
				return false
			}
			current = current.next
		}
	}
	return true
}

// Print - вывод множества
func (set *Set) Print() {
	fmt.Print("{")
	first := true

	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			if !first {
				fmt.Print(", ")
			}
			fmt.Printf("%d", current.key)
			first = false
			current = current.next
		}
	}
	fmt.Println("}")
}

// SaveToFile - сохранение множества в файл
func (set *Set) SaveToFile(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("ошибка открытия файла для записи: %w", err)
	}
	defer file.Close()

	for i := 0; i < set.tableSize; i++ {
		current := set.buckets[i]
		for current != nil {
			_, err := fmt.Fprintf(file, "%d\n", current.key)
			if err != nil {
				return fmt.Errorf("ошибка записи в файл: %w", err)
			}
			current = current.next
		}
	}

	return nil
}

// LoadFromFile - загрузка множества из файл
func (set *Set) LoadFromFile(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("ошибка открытия файла для чтения: %w", err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if line != "" {
			key, err := strconv.Atoi(line)
			if err != nil {
				return fmt.Errorf("ошибка преобразования числа: %w", err)
			}
			set.Insert(key)
		}
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("ошибка чтения файла: %w", err)
	}

	return nil
}

// PartitionBacktrack - рекурсивная функция backtracking для разбиения
func partitionBacktrack(nums []int, subsets [][]int, subsetSums []int, target, index int) bool {
	if index == len(nums) {
		// Проверяем, что все подмножества имеют целевую сумму
		for _, sum := range subsetSums {
			if sum != target {
				return false
			}
		}
		return true
	}

	num := nums[index]

	// Пробуем добавить число в каждое подмножество
	for i := 0; i < len(subsets); i++ {
		if subsetSums[i]+num <= target {
			// Добавляем число в подмножество
			subsets[i] = append(subsets[i], num)
			subsetSums[i] += num

			if partitionBacktrack(nums, subsets, subsetSums, target, index+1) {
				return true
			}

			// Откатываем изменения
			subsets[i] = subsets[i][:len(subsets[i])-1]
			subsetSums[i] -= num

			// Если подмножество пустое, не пробуем следующие пустые подмножества
			if subsetSums[i] == 0 {
				break
			}
		}
	}

	return false
}

// PartitionSetImproved - разбиение множества на подмножества с заданной суммой
func (set *Set) PartitionSetImproved(subsetSum int) ([][]int, bool) {
	nums := set.ToSlice()
	var totalSum int
	for _, num := range nums {
		totalSum += num
	}

	// Проверяем возможность разбиения
	if totalSum%subsetSum != 0 {
		fmt.Printf("Невозможно разбить множество: общая сумма %d не делится нацело на %d\n", totalSum, subsetSum)
		return nil, false
	}

	k := totalSum / subsetSum // количество подмножеств

	// Сортируем в порядке убывания для оптимизации
	sort.Slice(nums, func(i, j int) bool {
		return nums[i] > nums[j]
	})

	// Проверяем, что максимальный элемент не превышает целевую сумму
	if len(nums) > 0 && nums[0] > subsetSum {
		fmt.Printf("Невозможно разбить множество: элемент %d превышает целевую сумму %d\n", nums[0], subsetSum)
		return nil, false
	}

	// Инициализируем подмножества
	subsets := make([][]int, k)
	for i := range subsets {
		subsets[i] = make([]int, 0)
	}
	subsetSums := make([]int, k)

	if partitionBacktrack(nums, subsets, subsetSums, subsetSum, 0) {
		return subsets, true
	}

	fmt.Printf("Невозможно разбить множество на %d подмножеств с суммой %d\n", k, subsetSum)
	return nil, false
}