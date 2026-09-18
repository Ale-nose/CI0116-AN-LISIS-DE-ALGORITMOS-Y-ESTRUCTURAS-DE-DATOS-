#pragma once

/**
 * @brief Economic settings and initial values for the game.
 */
constexpr int STARTING_CREDITS = 300;
constexpr int STARTING_LIVES = 20;
constexpr int LEAK_PENALTY = 1;  // lives lost per enemy that reaches the base
constexpr int INTEREST_PERCENT = 5;  // applied once per wave, on unspent credits
