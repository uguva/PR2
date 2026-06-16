#include <iostream>
#include <string>
#include <fstream>
#include <clocale>
#include <vector>

#include "CryptoMath.h"
#include "RSA.h"
#include "InputUtils.h"

enum class MenuOption {
    POW = 1,
    EEA_MULT = 2,
    EEA_INV = 3,
    RSA_AND_MITM = 4,
    DIOPHANTINE = 6,
    EXIT = 0
};

void printMenu() {
    std::cout << "\n================ Практическая работа №2 ================\n";
    std::cout << "1. Задание 1: Возведение в степень (Бинарное и Ферма)\n";
    std::cout << "2. Задание 2: Таблица для c*d mod m = 1\n";
    std::cout << "3. Задание 3: Таблица для взаимообратного C^-1 mod m\n";
    std::cout << "4. Задания 4 и 5: RSA (Шифрование, Дешифрование, MitM)\n";
    std::cout << "6. Задание 6: Диофантово уравнение 237a + 44b = 1\n";
    std::cout << "0. Выход\n";
}

int main() {

    setlocale(LC_ALL, "ru_RU.UTF-8");

    while (true) {
        printMenu();
        int choice = getValidNumber<int>("Выберите опцию: ");
        
        MenuOption option = static_cast<MenuOption>(choice);
        if (option == MenuOption::EXIT) break;

        switch (option) {
            case MenuOption::POW: {
                int64_t a = getValidNumber<int64_t>("Введите основание (a): ");
                
                int64_t x;
                do {
                    x = getValidNumber<int64_t>("Введите степень (x >= 0): ");
                    if (x < 0) std::cout << "Ошибка: степень не может быть отрицательной!\n";
                } while (x < 0);
                
                int64_t p;
                do {
                    p = getValidNumber<int64_t>("Введите простой модуль (p): ");
                    if (p <= 1 || !isPrime(p)) std::cout << "Ошибка: модуль должен быть простым числом!\n";
                } while (p <= 1 || !isPrime(p));
                
                std::cout << "\n--- 1. Бинарное возведение по шагам ---\n";
                int64_t res1 = binPow(a, x, p, true);
                std::cout << "Итог: " << res1 << "\n\n";
                
                std::cout << "--- 2. Теорема Ферма (Последовательное умножение) ---\n";
                int64_t res2 = fermatPowSequential(a, x, p);
                if (res2 != -1) std::cout << "Итог: " << res2 << "\n";
                break;
            }

            case MenuOption::EEA_MULT: {
                int64_t c = getValidNumber<int64_t>("Введите c: ");
                int64_t m = getValidNumber<int64_t>("Введите m: ");
                int64_t x, y;
                std::cout << "\nВычисление НОД и коэффициентов:\n";
                
                gcdExtendedTable(c, m, x, y, true); 
                break;
            }

            case MenuOption::EEA_INV: {
                int64_t C = getValidNumber<int64_t>("Введите C: ");
                int64_t m = getValidNumber<int64_t>("Введите m: ");
                std::cout << "\nВычисление обратного элемента:\n";
    
                int64_t d = modInverse(C, m, true);
                if (d != -1) std::cout << "Обратное число d = " << d << "\n";
                break;
            }

case MenuOption::RSA_AND_MITM: {
                int action = getValidChoice("1 - Шифрование, 2 - Дешифрование, 3 - Эмуляция атаки MitM: ", 1, 3);
                
                if (action == 1) { 
                    int keySource = getValidChoice("1 - Создать новые ключи, 2 - Использовать Открытый ключ собеседника: ", 1, 2);
                    RSAKey pubKeyToUse;

                    if (keySource == 1) {
                        int64_t p, q;
                        do {
                            p = getValidNumber<int64_t>("Введите простое число p: ");
                            if (!isPrime(p)) std::cout << "Ошибка: число должно быть простым!\n";
                        } while (!isPrime(p));

                        do {
                            q = getValidNumber<int64_t>("Введите простое число q: ");
                            if (!isPrime(q)) std::cout << "Ошибка: число должно быть простым!\n";
                            else if (p == q) std::cout << "Рекомендация: p и q не должны быть равны.\n";
                        } while (!isPrime(q));

                        RSAKeyPair keys = generateRSAKeys(p, q);
                        pubKeyToUse = keys.publicKey;

                        int mode = getValidChoice("1 - Работа с консолью, 2 - Работа с файлом, 3 - Выход в меню: ", 1, 3);
                        if (mode == 3) {
                            std::cout << "Ключи успешно созданы. Возврат в главное меню.\n";
                            break;
                        }

                        std::string textToEncrypt;
                        if (mode == 1) {
                            textToEncrypt = getValidString("Введите текст для шифрования: ");
                            std::string filename = getValidString("Введите название файла для сохранения (без расширения): ");
                            rsaEncryptToFile(textToEncrypt, filename + ".bin", pubKeyToUse);
                        } 
                        else if (mode == 2) {
                            int fileAction = getValidChoice("1 - Открыть существующий файл, 2 - Создать новый: ", 1, 2);
                            std::string sourceFile;
                            if (fileAction == 1) {
                                while (true) {
                                    sourceFile = getValidString("Введите имя существующего текстового файла: ");
                                    std::ifstream in(sourceFile);
                                    if (in) {
                                        std::getline(in, textToEncrypt);
                                        std::cout << "Текст из файла: " << textToEncrypt << "\n";
                                        break;
                                    } else {
                                        std::cout << "Ошибка: Файл не найден! Попробуйте снова.\n"; 
                                    }
                                }
                            } else {
                                sourceFile = getValidString("Введите имя нового текстового файла (с расширением): ");
                                std::ofstream out(sourceFile);
                                textToEncrypt = getValidString("Введите текст: ");
                                out << textToEncrypt;
                                out.close();
                            }
                            
                            std::string destFile = getValidString("Введите название файла для сохранения шифра (без расширения): ");
                            rsaEncryptToFile(textToEncrypt, destFile + ".bin", pubKeyToUse);
                        }
                    } 
                    else if (keySource == 2) {
                        pubKeyToUse.part = getValidNumber<int64_t>("Введите открытый ключ собеседника (e): ");
                        pubKeyToUse.n = getValidNumber<int64_t>("Введите модуль собеседника (n): ");

                        int mode = getValidChoice("1 - Работа с консолью, 2 - Работа с файлом: ", 1, 2);
                        std::string textToEncrypt;
                        if (mode == 1) {
                            textToEncrypt = getValidString("Введите текст для шифрования: ");
                            std::string filename = getValidString("Введите название файла для сохранения (без расширения): ");
                            rsaEncryptToFile(textToEncrypt, filename + ".bin", pubKeyToUse);
                        } 
                        else if (mode == 2) {
                            int fileAction = getValidChoice("1 - Открыть существующий файл, 2 - Создать новый: ", 1, 2);
                            std::string sourceFile;
                            if (fileAction == 1) {
                                while (true) {
                                    sourceFile = getValidString("Введите имя существующего текстового файла: ");
                                    std::ifstream in(sourceFile);
                                    if (in) {
                                        std::getline(in, textToEncrypt);
                                        std::cout << "Текст из файла: " << textToEncrypt << "\n";
                                        break;
                                    } else {
                                        std::cout << "Ошибка: Файл не найден! Попробуйте снова.\n"; 
                                    }
                                }
                            } else {
                                sourceFile = getValidString("Введите имя нового текстового файла (с расширением): ");
                                std::ofstream out(sourceFile);
                                textToEncrypt = getValidString("Введите текст: ");
                                out << textToEncrypt;
                                out.close();
                            }
                            
                            std::string destFile = getValidString("Введите название файла для сохранения шифра (без расширения): ");
                            rsaEncryptToFile(textToEncrypt, destFile + ".bin", pubKeyToUse);
                        }
                    }
                } 
                else if (action == 2) { 
                    int mode = getValidChoice("1 - Вывод в консоль, 2 - Вывод в отдельный файл: ", 1, 2);
                    
                    std::vector<int64_t> cipher;
                    std::string encFile;
                    while (true) {
                        encFile = getValidString("Введите имя зашифрованного файла (с расширением .bin): ");
                        cipher = readCipherFromFile(encFile);
                        if (!cipher.empty()) break;
                        std::cout << "Попробуйте ввести имя файла заново.\n";
                    }
                    
                    int64_t d = getValidNumber<int64_t>("Введите ваш закрытый ключ (d): ");
                    int64_t n = getValidNumber<int64_t>("Введите ваш модуль (n): ");
                    RSAKey privKey = {d, n};
                    
                    std::string decryptedText = rsaDecryptString(cipher, privKey);
                    
                    if (mode == 1) {
                        std::cout << "\nРасшифрованный текст: " << decryptedText << "\n";
                    } else {
                        std::string destFile = getValidString("Введите имя текстового файла для сохранения расшифрованного текста: ");
                        std::ofstream out(destFile);
                        out << decryptedText;
                        std::cout << "Текст успешно сохранен!\n";
                    }
                }
                else if (action == 3) {
                    emulateRSAMitM();
                }
                break;
            }
            case MenuOption::DIOPHANTINE: {
                solveDiophantine(237, 44, 1);
                break;
            }
            default:
                std::cout << "Неверный ввод! Пожалуйста, выберите существующий пункт меню.\n";
        }
    }
    return 0;
}
