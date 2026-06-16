#include <iostream>
#include <string>
#include <fstream>
#include <clocale>

#include "CryptoMath.h"
#include "RSA.h"

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
    std::cout << "Выберите опцию: ";
}

int main() {

    setlocale(LC_ALL, "ru_RU.UTF-8");

    int choice;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) break;
        
        MenuOption option = static_cast<MenuOption>(choice);
        if (option == MenuOption::EXIT) break;

        switch (option) {
            case MenuOption::POW: {
                int64_t a, x, p;
                std::cout << "Введите основание (a), степень (x) и простой модуль (p): ";
                std::cin >> a >> x >> p;
                
                std::cout << "\n--- 1. Бинарное возведение по шагам ---\n";
                int64_t res1 = binPow(a, x, p, true);
                std::cout << "Итог: " << res1 << "\n\n";
                
                std::cout << "--- 2. Теорема Ферма (Последовательное умножение) ---\n";
                int64_t res2 = fermatPowSequential(a, x, p);
                if (res2 != -1) std::cout << "Итог: " << res2 << "\n";
                break;
            }

            case MenuOption::EEA_MULT: {
                int64_t c, m, x, y;
                std::cout << "Введите c и m: ";
                std::cin >> c >> m;
                std::cout << "\nВычисление НОД и коэффициентов:\n";
                
                gcdExtendedTable(c, m, x, y, true); 
                break;
            }

            case MenuOption::EEA_INV: {
                int64_t C, m;
                std::cout << "Введите C и m: ";
                std::cin >> C >> m;
                std::cout << "\nВычисление обратного элемента:\n";
    
                int64_t d = modInverse(C, m, true);
                if (d != -1) std::cout << "Обратное число d = " << d << "\n";
                break;
}


            case MenuOption::RSA_AND_MITM: {
                int action;
                std::cout << "1 - Шифрование, 2 - Дешифрование, 3 - Эмуляция атаки MitM: ";
                std::cin >> action;
                
                if (action == 1) { // === РЕЖИМ ШИФРОВАНИЯ ===
                    int keySource;
                    std::cout << "1 - Создать новые ключи (ввод p и q), 2 - Использовать Открытый ключ собеседника (ввод e и n): ";
                    std::cin >> keySource;

                    RSAKey pubKeyToUse; // Ключ, которым будем шифровать

                    if (keySource == 1) {
                        int64_t p = 0, q = 0;
                        do {
                            std::cout << "Введите простое число p: ";
                            std::cin >> p;
                            if (!isPrime(p)) std::cout << "Ошибка: число должно быть простым!\n";
                        } while (!isPrime(p));

                        do {
                            std::cout << "Введите простое число q: ";
                            std::cin >> q;
                            if (!isPrime(q)) std::cout << "Ошибка: число должно быть простым!\n";
                            if (p == q) std::cout << "Рекомендация: p и q не должны быть равны.\n";
                        } while (!isPrime(q));

                        RSAKeyPair keys = generateRSAKeys(p, q);
                        pubKeyToUse = keys.publicKey;

                        // Меню выбора режима работы после генерации
                        int mode;
                        std::cout << "1 - Работа с консолью, 2 - Работа с файлом, 3 - Остановиться на создании ключа (Выход в меню): ";
                        std::cin >> mode;

                        if (mode == 3) {
                            std::cout << "Ключи успешно созданы. Возврат в главное меню.\n";
                            break; // возвращает в цикл главного меню
                        }

                        std::string textToEncrypt;
                        if (mode == 1) {
                            std::cout << "Введите текст для шифрования: ";
                            std::cin.ignore();
                            std::getline(std::cin, textToEncrypt);
                            
                            std::string filename;
                            std::cout << "Введите название файла для сохранения (без расширения): ";
                            std::cin >> filename;
                            rsaEncryptToFile(textToEncrypt, filename + ".txt", pubKeyToUse);
                        } 
                        else if (mode == 2) {
                            int fileAction;
                            std::cout << "1 - Открыть существующий файл, 2 - Создать новый: ";
                            std::cin >> fileAction;
                            
                            std::string sourceFile;
                            if (fileAction == 1) {
                                std::cout << "Введите имя существующего файла: ";
                                std::cin >> sourceFile;
                                std::ifstream in(sourceFile);
                                if (in) {
                                    std::getline(in, textToEncrypt);
                                    std::cout << "Текст из файла: " << textToEncrypt << "\n";
                                } else {
                                    std::cout << "Файл не найден!\n"; 
                                    break;
                                }
                            } else {
                                std::cout << "Введите имя нового файла: ";
                                std::cin >> sourceFile;
                                std::ofstream out(sourceFile);
                                std::cout << "Введите текст: ";
                                std::cin.ignore();
                                std::getline(std::cin, textToEncrypt);
                                out << textToEncrypt;
                                out.close();
                            }
                            
                            std::string destFile;
                            std::cout << "Введите название файла для сохранения зашифрованных данных (без расширения): ";
                            std::cin >> destFile;
                            rsaEncryptToFile(textToEncrypt, destFile + ".txt", pubKeyToUse);
                        }
                        else {
                            std::cout << "Неверный выбор режима!\n";
                        }
                    } 
                    else if (keySource == 2) {
                        int64_t e, n;
                        std::cout << "Введите открытый ключ собеседника (e и n) через пробел: ";
                        std::cin >> e >> n;
                        pubKeyToUse = {e, n};

                        int mode;
                        std::cout << "1 - Работа с консолью, 2 - Работа с файлом: ";
                        std::cin >> mode;
                        
                        std::string textToEncrypt;
                        if (mode == 1) {
                            std::cout << "Введите текст для шифрования: ";
                            std::cin.ignore();
                            std::getline(std::cin, textToEncrypt);
                            
                            std::string filename;
                            std::cout << "Введите название файла для сохранения (без расширения): ";
                            std::cin >> filename;
                            rsaEncryptToFile(textToEncrypt, filename + ".txt", pubKeyToUse);
                        } 
                        else if (mode == 2) {
                            int fileAction;
                            std::cout << "1 - Открыть существующий файл, 2 - Создать новый: ";
                            std::cin >> fileAction;
                            
                            std::string sourceFile;
                            if (fileAction == 1) {
                                std::cout << "Введите имя существующего файла: ";
                                std::cin >> sourceFile;
                                std::ifstream in(sourceFile);
                                if (in) {
                                    std::getline(in, textToEncrypt);
                                    std::cout << "Текст из файла: " << textToEncrypt << "\n";
                                } else {
                                    std::cout << "Файл не найден!\n"; 
                                    break;
                                }
                            } else {
                                std::cout << "Введите имя нового файла: ";
                                std::cin >> sourceFile;
                                std::ofstream out(sourceFile);
                                std::cout << "Введите текст: ";
                                std::cin.ignore();
                                std::getline(std::cin, textToEncrypt);
                                out << textToEncrypt;
                                out.close();
                            }
                            
                            std::string destFile;
                            std::cout << "Введите название файла для сохранения зашифрованных данных (без расширения): ";
                            std::cin >> destFile;
                            rsaEncryptToFile(textToEncrypt, destFile + ".txt", pubKeyToUse);
                        }
                    } 
                    else {
                        std::cout << "Неверный выбор источника ключей!\n";
                        break;
                    }
                } 
                else if (action == 2) { // === РЕЖИМ ДЕШИФРОВАНИЯ ===
                    int mode;
                    std::cout << "1 - Вывод в консоль, 2 - Вывод в отдельный файл: ";
                    std::cin >> mode;
                    
                    std::string encFile;
                    std::cout << "Введите имя зашифрованного файла (с расширением): ";
                    std::cin >> encFile;
                    
                    std::vector<int64_t> cipher = readCipherFromFile(encFile);
                    if (cipher.empty()) break;
                    
                    int64_t d, n;
                    std::cout << "Введите ваш закрытый ключ (d и n) через пробел: ";
                    std::cin >> d >> n;
                    RSAKey privKey = {d, n};
                    
                    std::string decryptedText = rsaDecryptString(cipher, privKey);
                    
                    if (mode == 1) {
                        std::cout << "\nРасшифрованный текст: " << decryptedText << "\n";
                    } else {
                        std::string destFile;
                        std::cout << "Введите имя файла для сохранения расшифрованного текста: ";
                        std::cin >> destFile;
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
                std::cout << "Неверный ввод!\n";
        }
    }
    return 0;
}
