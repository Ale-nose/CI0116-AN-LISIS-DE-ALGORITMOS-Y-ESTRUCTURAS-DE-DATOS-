#pragma once
#include <cstdint>
#include <random>


// Constantes que venian en las especificaciones
constexpr int TICKS_PER_SECOND  = 60;
constexpr int MS_PER_TICK       = 1000 / TICKS_PER_SECOND;
constexpr int STEPS_PER_TICK    = 40;
constexpr int MAX_CATCHUP_TICKS = 5;
constexpr int SLOTS             = 8;

/**
 * @brief clase encargada de manejar los ticks dentro del juego
 * 
 */
class Ticks {
public:
    explicit Ticks(std::uint32_t seed); // usamos ints de 32 para que el rgn sea determinista, si usaramos un int normal no se asegura esto

    void advance(int elapsed_ms);  // llamado desde afuera con el tiempo real transcurrido
    void tick();                   // un tick de simulación
    bool over() const;

private:
    std::mt19937 rng_;
    int accumulator_ms_ = 0;
    int busy_ticks_[SLOTS] = {0};
    bool game_over_ = false;

    // ceil(steps / STEPS_PER_TICK)
    static int ticksToBlock(int steps) {
        return (steps + STEPS_PER_TICK - 1) / STEPS_PER_TICK;
    }
};