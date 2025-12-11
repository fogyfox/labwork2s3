package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// Database - база данных множеств
type Database struct {
	sets map[string]*Set
}

// NewDatabase - создание новой базы данных
func NewDatabase() *Database {
	return &Database{
		sets: make(map[string]*Set),
	}
}

// SaveToFile - сохранение базы данных в файл
func (db *Database) SaveToFile(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("ошибка открытия файла для записи: %w", err)
	}
	defer file.Close()

	for name, set := range db.sets {
		_, err := fmt.Fprintf(file, "SET %s ", name)
		if err != nil {
			return err
		}

		elements := set.ToSlice()
		for _, elem := range elements {
			_, err := fmt.Fprintf(file, "%d ", elem)
			if err != nil {
				return err
			}
		}
		_, err = fmt.Fprintln(file)
		if err != nil {
			return err
		}
	}

	fmt.Printf("Данные сохранены в файл: %s\n", filename)
	return nil
}

// LoadFromFile - загрузка базы данных из файла
func (db *Database) LoadFromFile(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		fmt.Printf("Файл '%s' не найден. Будет создан новый.\n", filename)
		return nil // файл может не существовать при первом запуске
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	loadedCount := 0

	for scanner.Scan() {
		line := scanner.Text()
		tokens := strings.Fields(line)
		if len(tokens) < 2 {
			continue
		}

		typeName := tokens[0]
		name := tokens[1]

		if typeName == "SET" {
			// Удаляем старое множество, если существует
			if _, exists := db.sets[name]; exists {
				DestroySet(db.sets[name])
				delete(db.sets, name)
			}

			db.sets[name] = NewSet(101)

			for i := 2; i < len(tokens); i++ {
				value, err := strconv.Atoi(tokens[i])
				if err != nil {
					fmt.Printf("Предупреждение: неверный элемент '%s' в множестве '%s' - пропущен\n", tokens[i], name)
					continue
				}
				db.sets[name].Insert(value)
			}
			loadedCount++
		}
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("ошибка чтения файла: %w", err)
	}

	fmt.Printf("Загружено %d множеств из файла: %s\n", loadedCount, filename)
	return nil
}

// ProcessSetQuery - обработка базовых операций с множеством
func (db *Database) ProcessSetQuery(tokens []string) {
	if len(tokens) < 2 {
		fmt.Println("Недостаточно аргументов для команды множества")
		return
	}

	command := tokens[0]
	setName := tokens[1]

	// Создаем множество, если не существует
	if _, exists := db.sets[setName]; !exists {
		db.sets[setName] = NewSet(101)
	}

	set := db.sets[setName]

	switch command {
	case "SINSERT":
		if len(tokens) < 3 {
			fmt.Println("SINSERT требует значение")
			return
		}
		value, err := strconv.Atoi(tokens[2])
		if err != nil {
			fmt.Println("Неверный числовой формат:", tokens[2])
			return
		}
		if set.Insert(value) {
			fmt.Println("OK")
		} else {
			fmt.Println("Элемент уже существует")
		}

	case "SCONTAINS":
		if len(tokens) < 3 {
			fmt.Println("SCONTAINS требует значение")
			return
		}
		value, err := strconv.Atoi(tokens[2])
		if err != nil {
			fmt.Println("Неверный числовой формат:", tokens[2])
			return
		}
		exists := set.Contains(value)
		if exists {
			fmt.Println("TRUE")
		} else {
			fmt.Println("FALSE")
		}

	case "SREMOVE":
		if len(tokens) < 3 {
			fmt.Println("SREMOVE требует значение")
			return
		}
		value, err := strconv.Atoi(tokens[2])
		if err != nil {
			fmt.Println("Неверный числовой формат:", tokens[2])
			return
		}
		if set.Remove(value) {
			fmt.Println("OK")
		} else {
			fmt.Println("Элемент не найден")
		}

	case "SSIZE":
		fmt.Println(set.Size())

	case "SEMPTY":
		if set.Empty() {
			fmt.Println("TRUE")
		} else {
			fmt.Println("FALSE")
		}

	case "SCLEAR":
		set.Clear()
		fmt.Println("OK")

	default:
		fmt.Println("Неизвестная команда:", command)
	}
}

// ProcessSetOperation - обработка операций между множествами
func (db *Database) ProcessSetOperation(tokens []string) {
	if len(tokens) < 3 {
		fmt.Println("Недостаточно аргументов для операции с множествами")
		return
	}

	command := tokens[0]
	resultSetName := tokens[1]
	set1Name := tokens[2]
	set2Name := ""
	if len(tokens) > 3 {
		set2Name = tokens[3]
	}

	// Проверяем существование исходных множеств
	set1, exists := db.sets[set1Name]
	if !exists {
		fmt.Printf("Множество '%s' не найдено\n", set1Name)
		return
	}

	var set2 *Set
	if set2Name != "" {
		var exists bool
		set2, exists = db.sets[set2Name]
		if !exists {
			fmt.Printf("Множество '%s' не найдено\n", set2Name)
			return
		}
	}

	var result *Set

	switch command {
	case "SUNION":
		if set2 == nil {
			fmt.Println("SUNION требует два множества")
			return
		}
		result = set1.Union(set2)

	case "SINTERSECTION":
		if set2 == nil {
			fmt.Println("SINTERSECTION требует два множества")
			return
		}
		result = set1.Intersection(set2)

	case "SDIFFERENCE":
		if set2 == nil {
			fmt.Println("SDIFFERENCE требует два множества")
			return
		}
		result = set1.Difference(set2)

	case "SSUBSET":
		if set2 == nil {
			fmt.Println("SSUBSET требует два множества")
			return
		}
		isSub := set1.IsSubset(set2)
		if isSub {
			fmt.Println("TRUE")
		} else {
			fmt.Println("FALSE")
		}
		return // не создаем новое множество для SUBSET

	default:
		fmt.Println("Неизвестная команда:", command)
		return
	}

	// Сохраняем результат
	if _, exists := db.sets[resultSetName]; exists {
		DestroySet(db.sets[resultSetName])
	}
	db.sets[resultSetName] = result
	fmt.Println("OK")
}

// ProcessPartitionOperation - обработка операции разбиения множества
func (db *Database) ProcessPartitionOperation(tokens []string) {
	if len(tokens) < 4 {
		fmt.Println("Использование: SPARTITION <исходное_множество> <сумма_подмножества> <префикс_результата>")
		return
	}

	sourceSetName := tokens[1]
	targetSum, err := strconv.Atoi(tokens[2])
	if err != nil {
		fmt.Println("Неверный формат суммы:", tokens[2])
		return
	}
	resultPrefix := tokens[3]

	// Проверяем существование исходного множества
	sourceSet, exists := db.sets[sourceSetName]
	if !exists {
		fmt.Printf("Множество '%s' не найдено\n", sourceSetName)
		return
	}

	if sourceSet.Empty() {
		fmt.Println("Исходное множество пусто")
		return
	}

	fmt.Printf("Разбиение множества '%s' на подмножества с суммой %d...\n", sourceSetName, targetSum)

	partitions, ok := sourceSet.PartitionSetImproved(targetSum)
	if ok {
		fmt.Printf("Успешно создано %d подмножеств:\n", len(partitions))

		// Сохраняем результаты как отдельные множества
		for i, partition := range partitions {
			resultSetName := fmt.Sprintf("%s_%d", resultPrefix, i+1)

			// Удаляем старое множество, если существует
			if _, exists := db.sets[resultSetName]; exists {
				DestroySet(db.sets[resultSetName])
				delete(db.sets, resultSetName)
			}

			// Создаем новое множество
			db.sets[resultSetName] = NewSet(101)

			// Заполняем элементами подмножества
			for _, num := range partition {
				db.sets[resultSetName].Insert(num)
			}

			// Выводим информацию о подмножестве
			fmt.Printf("Подмножество %d (%s): {", i+1, resultSetName)
			for j, num := range partition {
				fmt.Printf("%d", num)
				if j < len(partition)-1 {
					fmt.Print(", ")
				}
			}
			fmt.Print("} (сумма = ")

			// Проверяем сумму
			actualSum := 0
			for _, num := range partition {
				actualSum += num
			}
			fmt.Printf("%d)\n", actualSum)
		}
		fmt.Println("OK")
	} else {
		fmt.Printf("ОШИБКА: Невозможно разбить множество на подмножества с суммой %d\n", targetSum)
	}
}

// ProcessPrintCommand - обработка команды печати
func (db *Database) ProcessPrintCommand(tokens []string) {
	if len(tokens) < 2 {
		fmt.Println("SPRINT требует имя множества")
		return
	}

	setName := tokens[1]
	set, exists := db.sets[setName]
	if !exists {
		fmt.Printf("Множество '%s' не найдено\n", setName)
		return
	}

	fmt.Printf("Множество '%s': ", setName)
	set.Print()
}

func main() {
	if len(os.Args) < 5 {
		fmt.Println("Использование:", os.Args[0], "--file <filename> --query '<command>'")
		os.Exit(1)
	}

	var filename, query string
	for i := 1; i < len(os.Args); i++ {
		if os.Args[i] == "--file" && i+1 < len(os.Args) {
			filename = os.Args[i+1]
			i++
		} else if os.Args[i] == "--query" && i+1 < len(os.Args) {
			query = os.Args[i+1]
			i++
		}
	}

	if filename == "" || query == "" {
		fmt.Println("Использование:", os.Args[0], "--file <filename> --query '<command>'")
		os.Exit(1)
	}

	// Создаем базу данных
	db := NewDatabase()
	defer func() {
		// Очистка памяти
		for _, set := range db.sets {
			DestroySet(set)
		}
	}()

	// Загрузка данных из файла
	if err := db.LoadFromFile(filename); err != nil {
		fmt.Printf("Ошибка загрузки файла: %v\n", err)
		os.Exit(1)
	}

	// Разбор и выполнение запроса
	tokens := strings.Fields(query)
	if len(tokens) == 0 {
		fmt.Println("Пустой запрос")
		os.Exit(1)
	}

	command := tokens[0]

	switch command {
	case "SINSERT", "SCONTAINS", "SREMOVE", "SSIZE", "SEMPTY", "SCLEAR":
		db.ProcessSetQuery(tokens)
	case "SUNION", "SINTERSECTION", "SDIFFERENCE", "SSUBSET":
		db.ProcessSetOperation(tokens)
	case "SPRINT":
		db.ProcessPrintCommand(tokens)
	case "SPARTITION":
		db.ProcessPartitionOperation(tokens)
	default:
		fmt.Printf("Неизвестная команда: %s\n", command)
		os.Exit(1)
	}

	// Сохранение данных в файл
	if err := db.SaveToFile(filename); err != nil {
		fmt.Printf("Ошибка сохранения файла: %v\n", err)
		os.Exit(1)
	}
}