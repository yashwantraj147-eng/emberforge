// ============================================================================
//  Emberforge — Centralized Random Number Generator
// ============================================================================
#pragma once

#include <random>

namespace emberforge {

class Random {
public:
    static std::mt19937& engine() {
        static std::mt19937 gen(std::random_device{}());
        return gen;
    }

    static void seed(unsigned int s) {
        engine().seed(s);
    }

    static int get(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }

    static double getReal(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(engine());
    }

    static bool chance(int percent) {
        return get(1, 100) <= percent;
    }
};

} // namespace emberforge

