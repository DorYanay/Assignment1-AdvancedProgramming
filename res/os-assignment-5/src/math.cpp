#include "math.h"

#include <cmath>

bool isPrime(unsigned int number)
{
    if (number < 2) {
        return false;
    }
    if (number < 4) {
        return true;
    }

    unsigned int n = ((unsigned int)std::sqrt((long double)number)) + 1;

    for (int i = 2; i <= n; ++i) {
        if (number % i == 0) {
            return false;
        }
    }

    return true;
}
