#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct RSAKey {
    int64_t part; // e или d
    int64_t n;
};

struct RSAKeyPair {
    RSAKey publicKey;
    RSAKey privateKey;
};

// Генерация ключей
RSAKeyPair generateRSAKeys(int64_t p, int64_t q);

// Базовые функции шифрования
std::vector<int64_t> rsaEncryptString(const std::string& text, const RSAKey& pubKey);
std::string rsaDecryptString(const std::vector<int64_t>& cipher, const RSAKey& privKey);

// Функции для работы с файлами
void rsaEncryptToFile(const std::string& text, const std::string& outputFile, const RSAKey& pubKey);
std::vector<int64_t> readCipherFromFile(const std::string& inputFile);

// Эмуляция атаки
void emulateRSAMitM();
