// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

/**
 * @brief Economic settings and initial values for the game.
 */
constexpr int STARTING_CREDITS = 300;
constexpr int STARTING_LIVES = 20;
constexpr int LEAK_PENALTY = 1;  // lives lost per enemy that reaches the base
// applied once per wave, on unspent credits
constexpr int INTEREST_PERCENT = 5;
