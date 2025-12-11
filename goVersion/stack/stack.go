package main

import "fmt"

// NodeS - узел стека
type NodeS struct {
	key  string
	next *NodeS
}

// Stack - структура стека
type Stack struct {
	head *NodeS
	size int
}

// Empty - проверка на пустоту
func (st *Stack) Empty() bool {
	return st.head == nil
}

// CreateStack - создание нового стека
func CreateStack() *Stack {
	return &Stack{
		head: nil,
		size: 0,
	}
}

// CreateNode - создание нового узла
func CreateNode(key string) *NodeS {
	return &NodeS{
		key:  key,
		next: nil,
	}
}

// Push - добавление элемента в стек
func (st *Stack) Push(data string) {
	newNode := CreateNode(data)
	newNode.next = st.head
	st.head = newNode
	st.size++
}

// Pop - удаление и возврат элемента с вершины стека
func (st *Stack) Pop() (string, bool) {
	if st.Empty() {
		return "", false
	}

	el := st.head
	st.head = st.head.next
	st.size--

	// Отсоединяем элемент
	el.next = nil
	return el.key, true
}

// Peek - просмотр вершины стека без удаления
func (st *Stack) Peek() (string, bool) {
	if st.Empty() {
		return "", false
	}
	return st.head.key, true
}

// PrintStack - вывод стека
func (st *Stack) PrintStack() {
	if st.Empty() {
		fmt.Println("Стек пустой")
		return
	}

	el := st.head
	fmt.Print("Вывод стека: ")
	for el != nil {
		fmt.Printf("%s ", el.key)
		el = el.next
	}
	fmt.Println()
}

// GetAtIndex - получение элемента по индексу
func (st *Stack) GetAtIndex(inx int) (string, bool) {
	if inx < 0 || inx >= st.size {
		return "", false
	}

	el := st.head
	for i := 0; i < inx; i++ {
		el = el.next
	}
	return el.key, true
}

// Size - получение размера стека
func (st *Stack) Size() int {
	return st.size
}

// Clear - очистка стека
func (st *Stack) Clear() {
	for !st.Empty() {
		st.Pop()
	}
}

// Reverse - реверс стека
func (st *Stack) Reverse() *Stack {
	reversed := CreateStack()
	temp := CreateStack()

	// Копируем исходный стек во временный
	for !st.Empty() {
		val, _ := st.Pop()
		temp.Push(val)
	}

	// Копируем из временного в новый (реверс)
	for !temp.Empty() {
		val, _ := temp.Pop()
		reversed.Push(val)
	}

	// Восстанавливаем исходный стек
	for !reversed.Empty() {
		val, _ := reversed.Pop()
		st.Push(val)
	}

	// Реверс еще раз для правильного порядка
	result := CreateStack()
	for !st.Empty() {
		val, _ := st.Pop()
		result.Push(val)
	}

	return result
}