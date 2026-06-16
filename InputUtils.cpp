#include "InputUtils.h"

int getValidChoice(const std::string& prompt, int minChoice, int maxChoice) {
    int choice;
    while (true) {
        choice = getValidNumber<int>(prompt);
        if (choice >= minChoice && choice <= maxChoice) {
            return choice;
        } else {
            std::cout << "Ошибка: выберите вариант от " << minChoice << " до " << maxChoice << ".\n";
        }
    }
}

std::string getValidString(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        bool hasChars = false;
        for (char c : input) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasChars = true;
                break;
            }
        }
        if (hasChars) return input;
        std::cout << "Ошибка: строка не может быть пустой.\n";
    }
}
