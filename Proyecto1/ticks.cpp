#include "ticks.hpp"

Ticks::Ticks(std::uint32_t seed) : rng_(seed) {}

void Ticks::advance(int elapsed_ms) {
    accumulator_ms_ += elapsed_ms;

    int ticks_run = 0;
    while (accumulator_ms_ >= MS_PER_TICK && ticks_run < MAX_CATCHUP_TICKS) {
        tick();
        accumulator_ms_ -= MS_PER_TICK;
        ++ticks_run;
    }
    // si sobró acumulador tras el tope, se descarta ese exceso de este
}

void Ticks::tick() {
    for (int i = 0; i < SLOTS; ++i) {
        if (busy_ticks_[i] > 0) {
            --busy_ticks_[i];
            continue;
        }


    }
}

bool Ticks::over() const {
    return game_over_;
}