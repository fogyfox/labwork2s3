package main

import (
	"fmt"
	"sort"
)

// NodeBST - структура узла бинарного дерева поиска
type NodeBST struct {
	key   int
	left  *NodeBST
	right *NodeBST
}

// CreateNodeBST - создание нового узла
func CreateNodeBST(key int) *NodeBST {
	return &NodeBST{
		key:   key,
		left:  nil,
		right: nil,
	}
}

// InsertNode - добавление узла в BST
func InsertNode(root *NodeBST, key int) *NodeBST {
	if root == nil {
		return CreateNodeBST(key)
	}

	if key < root.key {
		root.left = InsertNode(root.left, key)
	} else if key > root.key {
		root.right = InsertNode(root.right, key)
	}

	return root
}

// InorderTraversal - обход inorder для получения отсортированных значений
func InorderTraversal(root *NodeBST, values *[]int) {
	if root == nil {
		return
	}
	InorderTraversal(root.left, values)
	*values = append(*values, root.key)
	InorderTraversal(root.right, values)
}

// RestoreValues - восстановление правильных значений в дереве
func RestoreValues(root *NodeBST, correctValues []int, index *int) {
	if root == nil {
		return
	}
	RestoreValues(root.left, correctValues, index)
	root.key = correctValues[*index]
	*index++
	RestoreValues(root.right, correctValues, index)
}

// RestoreBST - основная функция восстановления BST
func RestoreBST(root *NodeBST) *NodeBST {
	if root == nil {
		return root
	}

	// 1. Получаем текущие значения в дереве
	var currentValues []int
	InorderTraversal(root, &currentValues)

	// 2. Сортируем значения (это будет правильный порядок для BST)
	correctValues := make([]int, len(currentValues))
	copy(correctValues, currentValues)
	sort.Ints(correctValues)

	// 3. Восстанавливаем значения в дереве, сохраняя структуру
	index := 0
	RestoreValues(root, correctValues, &index)

	return root
}

// InorderPrint - обход слева-направо (inorder)
func InorderPrint(root *NodeBST) {
	if root == nil {
		return
	}
	InorderPrint(root.left)
	fmt.Printf("%d ", root.key)
	InorderPrint(root.right)
}

// CreateTestTree - создание тестового дерева с двумя поменянными узлами
func CreateTestTree() *NodeBST {
	// Создаем корректное BST:
	//       4
	//      / \
	//     2   6
	//    / \ / \
	//   1  3 5  7

	var root *NodeBST
	root = InsertNode(root, 4)
	root = InsertNode(root, 2)
	root = InsertNode(root, 6)
	root = InsertNode(root, 1)
	root = InsertNode(root, 3)
	root = InsertNode(root, 5)
	root = InsertNode(root, 7)

	// Меняем два узла местами (например, 3 и 6)
	// Ищем узлы и меняем их значения
	node3 := root.left.right  // узел со значением 3
	node6 := root.right       // узел со значением 6

	// Меняем значения местами
	node3.key, node6.key = node6.key, node3.key

	return root
}

func main() {
	// Создаем дерево с двумя поменянными узлами
	root := CreateTestTree()

	fmt.Println("Дерево ДО восстановления:")
	fmt.Print("Inorder обход: ")
	InorderPrint(root)
	fmt.Println()
	fmt.Println()

	// Восстанавливаем дерево
	root = RestoreBST(root)
	fmt.Println("Дерево ПОСЛЕ восстановления:")
	fmt.Print("Inorder обход: ")
	InorderPrint(root)
	fmt.Println()
}