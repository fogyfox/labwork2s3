package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	calculator := &ExpressionCalculator{}
	reader := bufio.NewReader(os.Stdin)

	fmt.Println("Калькулятор математических выражений")
	fmt.Println("=====================================")
	fmt.Println("Поддерживаемые операции: +, -, *")
	fmt.Println("Поддерживаются скобки: (, )")
	fmt.Println("Максимальное значение: ±2,000,000,000")
	fmt.Println("Введите 'выход' для завершения")
	fmt.Println()

	for {
		fmt.Print("Введите математическое выражение: ")
		expression, _ := reader.ReadString('\n')
		expression = expression[:len(expression)-1] // Убираем символ новой строки

		if expression == "выход" || expression == "exit" {
			break
		}

		result, err := calculator.Evaluate(expression)
		if err != nil {
			fmt.Printf("Ошибка: %v\n", err)
		} else {
			fmt.Printf("Результат: %d\n", result)
		}
		fmt.Println()
	}

	fmt.Println("Программа завершена.")
}