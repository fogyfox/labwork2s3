#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
#include "stack.h"

using namespace std;

class ExpressionCalculator {
private:
    //проверка приоритета операторов
    int getPriority(const string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*") return 2;
        return 0;
    }

    //выполнение операции
    long long applyOperation(long long a, long long b, const string& op) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        throw runtime_error("Неизвестный оператор: " + op);
    }

    //проверка является ли строка оператором
    bool isOperator(const string& s) {
        return s == "+" || s == "-" || s == "*";
    }

    //проверка является ли строка числом (включая отрицательные)
    bool isNumber(const string& s) {
        if (s.empty()) return false;
        size_t start = 0;
        if (s[0] == '-') {
            if (s.length() == 1) return false; // только минус
            start = 1;
        }
        for (size_t i = start; i < s.length(); i++) {
            if (!isdigit(s[i])) return false;
        }
        return true;
    }

    //преобразование в постфиксную запись
    void toPostfix(const string& expression, stack& output) {
        stack operators;
        createStack(&operators);
        createStack(&output);

        string currentNumber;
        
        for (size_t i = 0; i < expression.length(); i++) {
            char c = expression[i];
            
            //пропускаем пробелы
            if (c == ' ') continue;
            
            //если цифра - собираем всё число
            if (isdigit(c) || (c == '-' && (i == 0 || expression[i-1] == '(' || 
                             isOperator(string(1, expression[i-1]))))) {
                currentNumber.clear();
                if (c == '-') {
                    currentNumber += c;
                    i++;
                }
                while (i < expression.length() && isdigit(expression[i])) {
                    currentNumber += expression[i];
                    i++;
                }
                i--; //возвращаемся на один символ назад
                
                push(&output, currentNumber);
            }
            //открывающая скобка
            else if (c == '(') {
                push(&operators, "(");
            }
            //закрывающая скобка
            else if (c == ')') {
                while (!empty(&operators) && operators.head->key != "(") {
                    push(&output, operators.head->key);
                    pop(&operators);
                }
                if (empty(&operators)) {
                    throw runtime_error("Несбалансированные скобки");
                }
                pop(&operators); //удаляем "("
            }
            //оператор
            else if (c == '+' || c == '-' || c == '*') {
                string op(1, c);
                while (!empty(&operators) && 
                       getPriority(operators.head->key) >= getPriority(op) &&
                       operators.head->key != "(") {
                    push(&output, operators.head->key);
                    pop(&operators);
                }
                push(&operators, op);
            }
            else {
                throw runtime_error("Неизвестный символ: " + string(1, c));
            }
        }
        
        //выталкиваем оставшиеся операторы
        while (!empty(&operators)) {
            if (operators.head->key == "(") {
                throw runtime_error("Несбалансированные скобки");
            }
            push(&output, operators.head->key);
            pop(&operators);
        }
    }

public:
    long long evaluate(const string& expression) {
        if (expression.empty()) {
            throw runtime_error("Пустое выражение");
        }

        stack postfix;
        toPostfix(expression, postfix);
        
        //вычисление постфиксного выражения
        stack calcStack;
        createStack(&calcStack);
        
        //временный стек для реверса
        stack temp;
        createStack(&temp);
        
        //реверс стека postfix
        while (!empty(&postfix)) {
            push(&temp, postfix.head->key);
            pop(&postfix);
        }
        
        //вычисление выражения
        while (!empty(&temp)) {
            string token = temp.head->key;
            pop(&temp);
            
            if (isNumber(token)) {
                push(&calcStack, token);
            } 
            else if (isOperator(token)) {
                if (calcStack.size < 2) {
                    throw runtime_error("Неверное выражение");
                }
                
                string val2Str = calcStack.head->key;
                pop(&calcStack);
                string val1Str = calcStack.head->key;
                pop(&calcStack);
                
                long long val2 = stoll(val2Str);
                long long val1 = stoll(val1Str);
                
                long long result = applyOperation(val1, val2, token);
                
                //проверка на переполнение
                if (result > 2000000000LL || result < -2000000000LL) {
                    throw runtime_error("Переполнение при вычислении");
                }
                
                push(&calcStack, to_string(result));
            }
            else {
                throw runtime_error("Неизвестный токен: " + token);
            }
        }
        
        if (calcStack.size != 1) {
            throw runtime_error("Неверное выражение");
        }
        
        long long result = stoll(calcStack.head->key);
        
        //финальная проверка диапазона
        if (result > 2000000000LL || result < -2000000000LL) {
            throw runtime_error("Результат превышает допустимое значение");
        }
        
        return result;
    }
};

int main() {
    ExpressionCalculator calculator;
    string expression;
    
    cout << "Введите математическое выражение: ";
    getline(cin, expression);
    
    try {
        long long result = calculator.evaluate(expression);
        cout << "Результат: " << result << endl;
    } catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
    
    return 0;
}