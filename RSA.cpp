#include "RSA.h"
#include "CryptoMath.h"
#include <iostream>
#include <fstream>

RSAKeyPair generateRSAKeys(int64_t p, int64_t q) {
    int64_t n = p * q;
    int64_t phi = (p - 1) * (q - 1);
    
    int64_t e = 3;
    while (gcd(e, phi) != 1) { e++; }
    int64_t d = modInverse(e, phi, false); // Без таблицы
    
    std::cout << "Открытым ключом является пара (e=" << e << ", n=" << n << ")\n";
    std::cout << "Закрытым ключом является пара (d=" << d << ", n=" << n << ")\n";
    
    return {{e, n}, {d, n}};
}

std::vector<int64_t> rsaEncryptString(const std::string& text, const RSAKey& pubKey) {
    std::vector<int64_t> cipher;
    for (char c : text) {
        int64_t m = static_cast<unsigned char>(c);
        cipher.push_back(binPow(m, pubKey.part, pubKey.n, false));
    }
    return cipher;
}

std::string rsaDecryptString(const std::vector<int64_t>& cipher, const RSAKey& privKey) {
    std::string text = "";
    for (int64_t c : cipher) {
        int64_t m = binPow(c, privKey.part, privKey.n, false);
        text += static_cast<char>(m);
    }
    return text;
}

void rsaEncryptToFile(const std::string& text, const std::string& outputFile, const RSAKey& pubKey) {
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Ошибка создания файла!\n";
        return;
    }
    std::vector<int64_t> cipher = rsaEncryptString(text, pubKey);
    for (int64_t c : cipher) {
        out.write(reinterpret_cast<const char*>(&c), sizeof(c));
    }
    std::cout << "Данные успешно зашифрованы и сохранены в файл: " << outputFile << "\n";
}

std::vector<int64_t> readCipherFromFile(const std::string& inputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    std::vector<int64_t> cipher;
    if (!in) {
        std::cerr << "Ошибка чтения файла!\n";
        return cipher;
    }
    int64_t val;
    while (in.read(reinterpret_cast<char*>(&val), sizeof(val))) {
        cipher.push_back(val);
    }
    return cipher;
}

void emulateRSAMitM() {
    std::cout << "\n--- Эмуляция атаки 'Человек посередине' (MitM) ---\n";
    std::cout << "[1] Алиса генерирует свои ключи (p=23, q=29)...\n";
    RSAKeyPair aliceKeys = generateRSAKeys(23, 29);
    
    std::cout << "\n[2] Мэллори (Хакер) перехватывает публичный ключ Алисы.\n";
    std::cout << "[3] Мэллори создает свои поддельные ключи (p=11, q=17) для Боба...\n";
    RSAKeyPair malloryKeys = generateRSAKeys(11, 17);
    
    std::cout << "\n[4] Боб получает ключ Мэллори и шифрует секрет 'SECRET'\n";
    std::string secret = "SECRET";
    std::vector<int64_t> bobCipher = rsaEncryptString(secret, malloryKeys.publicKey);
    
    std::cout << "\n[5] Мэллори перехватывает данные и расшифровывает СВОИМ закрытым ключом!\n";
    std::string intercepted = rsaDecryptString(bobCipher, malloryKeys.privateKey);
    std::cout << " -> Мэллори прочитал: " << intercepted << "\n";
    
    std::cout << "\n[6] Мэллори перешифровывает сообщение ключом Алисы и отправляет ей.\n";
    std::vector<int64_t> toAliceCipher = rsaEncryptString(intercepted, aliceKeys.publicKey);
    std::string aliceReads = rsaDecryptString(toAliceCipher, aliceKeys.privateKey);
    std::cout << " -> Алиса прочитала: " << aliceReads << " (Не зная о перехвате)\n";
}
