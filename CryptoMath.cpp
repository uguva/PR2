#include "CryptoMath.h"
#include <iostream>
#include <iomanip>

bool isPrime(int64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int64_t gcd(int64_t a, int64_t b) {
    while (b) {
        a %= b;
        std::swap(a, b);
    }
    return a;
}


int64_t binPow(int64_t base, int64_t exp, int64_t mod, bool verbose) {
    int64_t res = 1;
    base %= mod;
    
    if (verbose) {
        std::cout << "[Контекст] Идея метода: представляем степень в двоичном виде.\n"
                  << "           Если текущий младший бит равен 1 — умножаем результат на текущее основание.\n"
                  << "           В конце каждого шага основание всегда возводится в квадрат по модулю.\n\n";
    }

    int step = 1;
    while (exp > 0) {
        int bit = exp & 1;
        if (verbose) {
            std::cout << "Шаг " << step << ": Степень = " << exp << " (в двоичной форме младший бит = " << bit << ")\n";
        }
        
        if (bit) {
            res = (static_cast<__int64_t>(res) * base) % mod;
            if (verbose) {
                std::cout << " -> Бит равен 1: умножаем промежуточный результат на основание. Новое значение res = " << res << "\n";
            }
        } else {
            if (verbose) {
                std::cout << " -> Бит равен 0: умножение res не требуется (оставляем res = " << res << ")\n";
            }
        }
        
        if (verbose) {
            std::cout << " -> Подготовка к следующему шагу: возводим основание " << base << " в квадрат.\n";
        }
        
        base = (static_cast<__int64_t>(base) * base) % mod;
        exp >>= 1;
        
        if (verbose) {
            std::cout << "    Новое основание для следующего шага = " << base << ", Сдвинутая степень = " << exp << "\n";
            std::cout << "--------------------------------------------------------------------------------\n";
        }
        step++;
    }
    return res;
}

int64_t fermatPowSequential(int64_t base, int64_t exp, int64_t p) {
    if (!isPrime(p)) {
        std::cerr << "Ошибка: Модуль " << p << " не является простым!\n";
        return -1;
    }
    if (gcd(base, p) != 1) {
        std::cerr << "Ошибка: Число и модуль не взаимно простые!\n";
        return -1;
    }
    
    std::cout << "Подсказка: По Малой теореме Ферма a^(p-1) = 1 (mod p).\n";
    std::cout << "Поэтому мы можем отбросить 'целые циклы' (p-1) из нашей степени.\n";
    
    int64_t reduced_exp = exp % (p - 1);
    std::cout << "Степень " << exp << " упрощается до: " << exp << " mod (" << p << "-1) = " << reduced_exp << "\n\n";
    
    if (reduced_exp == 0) return 1;
    
    int64_t res = base % p;
    std::cout << "Шаг 1: Начальный результат = " << res << "\n";
    for (int64_t i = 2; i <= reduced_exp; ++i) {
        res = (res * base) % p;
        std::cout << "Шаг " << i << ": Умножаем на основание " << base << " -> Результат = " << res << "\n";
    }
    return res;
}

int64_t gcdExtendedTable(int64_t a, int64_t b, int64_t& x, int64_t& y, bool printTable) {
    int64_t r0 = a, r1 = b;
    int64_t u0 = 1, v0 = 0;
    int64_t u1 = 0, v1 = 1;
    int step = 1;

    // Сохраняем оригинальные значения для финальной проверки
    int64_t orig_a = a;
    int64_t orig_b = b;

    if (printTable) {
        std::cout << std::left 
                  << std::setw(5) << "Шаг" << " | "
                  << std::setw(10) << "a" << " | "
                  << std::setw(10) << "b" << " | "
                  << std::setw(10) << "a // b" << " | "
                  << std::setw(10) << "u" << " | "
                  << std::setw(10) << "v" << "\n";
        std::cout << "-----------------------------------------------------------------\n";
    }

    while (r1 != 0) {
        int64_t q = r0 / r1;
        if (printTable) {
            std::cout << std::left 
                      << std::setw(5) << step++ << " | "
                      << std::setw(10) << r0 << " | "
                      << std::setw(10) << r1 << " | "
                      << std::setw(10) << q << " | "
                      << std::setw(10) << u0 << " | "
                      << std::setw(10) << v0 << "\n";
        }
        int64_t r2 = r0 - q * r1;
        int64_t u2 = u0 - q * u1;
        int64_t v2 = v0 - q * v1;

        r0 = r1; r1 = r2;
        u0 = u1; u1 = u2;
        v0 = v1; v1 = v2;
    }
    
    if (printTable) {
        std::cout << std::left 
                  << std::setw(5) << step << " | "
                  << std::setw(10) << r0 << " | "
                  << std::setw(10) << r1 << " | "
                  << std::setw(10) << "-" << " | "
                  << std::setw(10) << u0 << " | "
                  << std::setw(10) << v0 << "\n";
                  
        std::cout << "\nПроверка равенством (a * u + b * v = НОД):\n";
        std::cout << orig_a << " * (" << u0 << ") + " << orig_b << " * (" << v0 << ") = " 
                  << (orig_a * u0 + orig_b * v0) << "\n";
        std::cout << "Результат: НОД = " << r0 << ", u = " << u0 << ", v = " << v0 << "\n\n";
    }

    x = u0;
    y = v0;
    return r0;
}

int64_t modInverse(int64_t a, int64_t m, bool printTable) {
    int64_t x, y;
    int64_t g = gcdExtendedTable(a, m, x, y, printTable);
    if (g != 1) {
        std::cout << "Обратного элемента не существует (НОД != 1)\n";
        return -1; 
    }
    return (x % m + m) % m;
}

void solveDiophantine(int64_t a, int64_t b, int64_t c) {
    int64_t x, y;
    int64_t g = gcdExtendedTable(a, b, x, y, true);
    
    if (c % g != 0) {
        std::cout << "Уравнение не имеет решений в целых числах!\n";
        return;
    }
    
    int64_t k_mul = c / g;
    int64_t x0 = x * k_mul;
    int64_t y0 = y * k_mul;
    
    std::cout << "Базовое решение: a0 = " << x0 << ", b0 = " << y0 << "\n";
    std::cout << "Общее решение:\n";
    std::cout << " a = " << x0 << " + " << (b / g) << " * k\n";
    std::cout << " b = " << y0 << " - " << (a / g) << " * k\n";
}
