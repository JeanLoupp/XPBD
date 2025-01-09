#pragma once

#include <chrono>

class Timer {
public:
    Timer() : last(std::chrono::high_resolution_clock::now()) {}

    void reset() {
        last = std::chrono::high_resolution_clock::now();
    }

    double elapsed() {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = now - last;
        last = now;
        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point last;
};