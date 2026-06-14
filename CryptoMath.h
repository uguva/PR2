#pragma once
#include <cstdint>

bool isPrime(int64_t n);
int64_t gcd(int64_t a, int64_t b);


int64_t binPow(int64_t base, int64_t exp, int64_t mod, bool verbose = false);
int64_t fermatPowSequential(int64_t base, int64_t exp, int64_t p);


int64_t gcdExtendedTable(int64_t a, int64_t b, int64_t& x, int64_t& y, bool printTable = true);
int64_t modInverse(int64_t a, int64_t m, bool printTable = true);
void solveDiophantine(int64_t a, int64_t b, int64_t c);
