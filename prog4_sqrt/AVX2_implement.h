#include <immintrin.h>
#include <iostream>
#include <unistd.h>
using namespace std;

void sqrt_implement_by_AVX2(int N,
    float initialGuess,
    float values[],
    float output[]);