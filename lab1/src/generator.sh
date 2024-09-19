#!/bin/bash

# Количество случайных чисел
count=150

# Создание файла numbers.txt и запись в него 150 случайных чисел
for ((i=0; i<count; i++)); do
    # Генерация случайного числа от 0 до 999
    random_number=$(od -An -N2 -i /dev/random | tr -d ' ')
    echo $random_number >> numbers.txt
done
