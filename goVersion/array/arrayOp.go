package main

import (
	"fmt"
	"strings"
)

// NodeA - узел массива
type NodeA struct {
	value string
}

// Array - структура динамического массива
type Array struct {
	data     []NodeA
	size     int
	capacity int
}

// CreateArray - создание нового массива
func CreateArray() *Array {
	return &Array{
		capacity: 1,
		data:     make([]NodeA, 1),
		size:     0,
	}
}

// CreateNodeAr - создание нового узла
func CreateNodeAr(value string) *NodeA {
	return &NodeA{value: value}
}

// PushBack - добавление элемента в конец массива
func (arr *Array) PushBack(value string) {
	if arr.size >= arr.capacity {
		arr.capacity *= 2
		newData := make([]NodeA, arr.capacity)
		copy(newData, arr.data[:arr.size])
		arr.data = newData
	}

	if arr.size < len(arr.data) {
		arr.data[arr.size] = NodeA{value: value}
	} else {
		arr.data = append(arr.data, NodeA{value: value})
	}
	arr.size++
}

// AddInx - добавление элемента по индексу
func (arr *Array) AddInx(value string, inx int) error {
	if inx < 0 || inx > arr.size {
		return fmt.Errorf("выход за диапазон")
	}

	if arr.size >= arr.capacity {
		arr.capacity *= 2
		newData := make([]NodeA, arr.capacity)
		copy(newData, arr.data[:arr.size])
		arr.data = newData
	}

	// Сдвигаем элементы вправо
	arr.data = append(arr.data, NodeA{})
	if inx < arr.size {
		copy(arr.data[inx+1:], arr.data[inx:arr.size])
	}
	arr.data[inx] = NodeA{value: value}
	arr.size++

	return nil
}

// PrintArray - вывод массива
func (arr *Array) PrintArray() {
	if arr.IsEmpty() {
		fmt.Println("Массив пустой")
		return
	}

	fmt.Print("Вывод массива: ")
	for i := 0; i < arr.size; i++ {
		fmt.Printf("%s ", arr.data[i].value)
	}
	fmt.Println()
}

// GetInx - получение элемента по индексу
func (arr *Array) GetInx(inx int) (string, error) {
	if inx < 0 || inx >= arr.size {
		return "", fmt.Errorf("выход за диапазон")
	}
	return arr.data[inx].value, nil
}

// ChangeInx - замена элемента по индексу
func (arr *Array) ChangeInx(newValue string, inx int) error {
	if inx < 0 || inx >= arr.size {
		return fmt.Errorf("выход за диапазон")
	}
	arr.data[inx].value = newValue
	return nil
}

// SizeArr - получение размера массива
func (arr *Array) SizeArr() int {
	return arr.size
}

// RemoveElArr - удаление элемента по индексу
func (arr *Array) RemoveElArr(inx int) (string, error) {
	if inx < 0 || inx >= arr.size {
		return "", fmt.Errorf("выход за диапазон")
	}

	removedValue := arr.data[inx].value

	// Сдвигаем элементы влево
	copy(arr.data[inx:], arr.data[inx+1:arr.size])
	arr.size--

	return removedValue, nil
}

// PrintAllSubarrays - вывод всех различных подмассивов
func (arr *Array) PrintAllSubarrays() {
	n := arr.size

	fmt.Println("Все различные подмассивы:")
	
	// Пустой подмассив
	fmt.Println("{}")

	// Генерируем все возможные подмассивы
	for start := 0; start < n; start++ {
		for end := start; end < n; end++ {
			var subarray []string
			for i := start; i <= end; i++ {
				subarray = append(subarray, arr.data[i].value)
			}
			fmt.Printf("{%s}\n", strings.Join(subarray, ", "))
		}
	}
}

// IsEmpty - проверка на пустоту
func (arr *Array) IsEmpty() bool {
	return arr.size == 0
}