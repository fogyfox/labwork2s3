package main

import "fmt"

func main() {
	arr := CreateArray()
	
	// добавляем элементы в массив
	arr.PushBack("x")
	arr.PushBack("y")
	arr.PushBack("z")
	
	arr.PrintArray()
	fmt.Println()
	
	// выводим все подмассивы
	arr.PrintAllSubarrays()
}