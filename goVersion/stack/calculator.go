package main

import (
	"fmt"
	"strconv"
	"strings"
	"unicode"
)

// ExpressionCalculator - калькулятор математических выражений
type ExpressionCalculator struct{}

// precedence - приоритет операторов
func (ec *ExpressionCalculator) precedence(op string) int {
	switch op {
	case "+", "-":
		return 1
	case "*":
		return 2
	}
	return 0
}

// applyOperation - выполнение операции
func (ec *ExpressionCalculator) applyOperation(a, b int64, op string) (int64, error) {
	switch op {
	case "+":
		return a + b, nil
	case "-":
		return a - b, nil
	case "*":
		return a * b, nil
	default:
		return 0, fmt.Errorf("неизвестный оператор: %s", op)
	}
}

// isOperator - проверка является ли строка оператором
func (ec *ExpressionCalculator) isOperator(s string) bool {
	return s == "+" || s == "-" || s == "*"
}

// Evaluate - вычисление выражения (исправленная версия)
func (ec *ExpressionCalculator) Evaluate(expression string) (int64, error) {
	// Удаляем все пробелы
	expression = strings.ReplaceAll(expression, " ", "")
	if len(expression) == 0 {
		return 0, fmt.Errorf("пустое выражение")
	}

	// Преобразуем в постфиксную запись
	postfixTokens, err := ec.infixToPostfix(expression)
	if err != nil {
		return 0, err
	}

	// Вычисляем постфиксное выражение
	return ec.evaluatePostfix(postfixTokens)
}

// infixToPostfix - преобразование инфиксной записи в постфиксную
func (ec *ExpressionCalculator) infixToPostfix(expression string) ([]string, error) {
	var output []string
	operators := CreateStack()

	i := 0
	for i < len(expression) {
		ch := expression[i]

		// Если цифра
		if unicode.IsDigit(rune(ch)) {
			// Собираем все цифры числа
			start := i
			for i < len(expression) && unicode.IsDigit(rune(expression[i])) {
				i++
			}
			output = append(output, expression[start:i])
			continue
		}

		// Обработка отрицательных чисел
		if ch == '-' && (i == 0 || expression[i-1] == '(' || ec.isOperator(string(expression[i-1]))) {
			// Это унарный минус, начинаем число с минуса
			start := i
			i++ // Пропускаем минус
			for i < len(expression) && unicode.IsDigit(rune(expression[i])) {
				i++
			}
			output = append(output, expression[start:i])
			continue
		}

		// Открывающая скобка
		if ch == '(' {
			operators.Push("(")
			i++
			continue
		}

		// Закрывающая скобка
		if ch == ')' {
			for !operators.Empty() {
				top, _ := operators.Peek()
				if top == "(" {
					break
				}
				op, _ := operators.Pop()
				output = append(output, op)
			}

			if operators.Empty() {
				return nil, fmt.Errorf("несбалансированные скобки")
			}

			operators.Pop() // Удаляем "("
			i++
			continue
		}

		// Оператор
		if ec.isOperator(string(ch)) {
			op := string(ch)
			for !operators.Empty() {
				top, _ := operators.Peek()
				if top == "(" || ec.precedence(top) < ec.precedence(op) {
					break
				}
				operator, _ := operators.Pop()
				output = append(output, operator)
			}
			operators.Push(op)
			i++
			continue
		}

		return nil, fmt.Errorf("неизвестный символ: %c", ch)
	}

	// Выталкиваем оставшиеся операторы
	for !operators.Empty() {
		op, _ := operators.Pop()
		if op == "(" {
			return nil, fmt.Errorf("несбалансированные скобки")
		}
		output = append(output, op)
	}

	return output, nil
}

// evaluatePostfix - вычисление постфиксного выражения
func (ec *ExpressionCalculator) evaluatePostfix(tokens []string) (int64, error) {
	stack := CreateStack()

	for _, token := range tokens {
		// Если число
		if _, err := strconv.ParseInt(token, 10, 64); err == nil {
			stack.Push(token)
		} else if ec.isOperator(token) {
			// Оператор
			if stack.Size() < 2 {
				return 0, fmt.Errorf("недостаточно операндов для оператора %s", token)
			}

			// Получаем операнды (внимание: порядок важен!)
			bStr, _ := stack.Pop()
			aStr, _ := stack.Pop()

			b, err1 := strconv.ParseInt(bStr, 10, 64)
			a, err2 := strconv.ParseInt(aStr, 10, 64)

			if err1 != nil || err2 != nil {
				return 0, fmt.Errorf("ошибка преобразования чисел")
			}

			result, err := ec.applyOperation(a, b, token)
			if err != nil {
				return 0, err
			}

			// Проверка на переполнение
			if result > 2000000000 || result < -2000000000 {
				return 0, fmt.Errorf("переполнение при вычислении")
			}

			stack.Push(strconv.FormatInt(result, 10))
		} else {
			return 0, fmt.Errorf("неизвестный токен: %s", token)
		}
	}

	if stack.Size() != 1 {
		return 0, fmt.Errorf("неверное выражение")
	}

	resultStr, _ := stack.Pop()
	result, err := strconv.ParseInt(resultStr, 10, 64)
	if err != nil {
		return 0, fmt.Errorf("ошибка преобразования результата")
	}

	return result, nil
}