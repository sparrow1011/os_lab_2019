#!/bin/bash

# Проверка на наличие аргументов
if [ $# -eq 0 ]; then
    echo "No arguments provided."
    exit 1
fi

# Инициализация переменных
sum=0
count=0

# Перебор всех аргументов
for arg in "$@"; do
    # Проверка, является ли аргумент числом
    if ! [[ $arg =~ ^-?[0-9]+([.][0-9]+)?$ ]]; then
        echo "Invalid argument: $arg"
        exit 1
    fi
    sum=$(echo "$sum + $arg" | bc)
    count=$((count + 1))
done

# Вычисление среднего арифметического
average=$(echo "$sum / $count" | bc -l)

# Вывод результатов
echo "Number of arguments: $count"
echo "Average: $average"
