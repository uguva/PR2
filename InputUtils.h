#pragma once
#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <type_traits>

// Шаблонная функция
template <typename T>
T getValidNumber(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, input)) {
            std::cin.clear();
            continue;
        }
        
        try {
            size_t pos;
            T value;
            if constexpr (std::is_same<T, int64_t>::value) {
                value = std::stoll(input, &pos);
            } else {
                value = std::stoi(input, &pos);
            }
            
            bool exactMatch = true;
            for (size_t i = pos; i < input.length(); ++i) {
                if (!std::isspace(static_cast<unsigned char>(input[i]))) {
                    exactMatch = false;
                    break;
                }
            }
            
            if (exactMatch) {
                return value;
            } else {
                std::cout << "Ошибка: обнаружены лишние буквы или символы. Введите только число.\n";
            }
        } catch (const std::invalid_argument&) {
            std::cout << "Ошибка: некорректный ввод. Пожалуйста, введите число.\n";
        } catch (const std::out_of_range&) {
            std::cout << "Ошибка: введенное число слишком большое или маленькое.\n";
        }
    }
}

// обычные функции
int getValidChoice(const std::string& prompt, int minChoice, int maxChoice);
std::string getValidString(const std::string& prompt);
