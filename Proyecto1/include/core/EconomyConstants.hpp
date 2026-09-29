// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

/**
 * @brief Credits the player starts the match with.
 */
constexpr int STARTING_CREDITS = 300;

/**
 * @brief Lives the player starts the match with.
 */
constexpr int STARTING_LIVES = 20;

/**
 * @brief Lives lost per enemy that reaches the base.
 */
constexpr int LEAK_PENALTY = 1;

/**
 * @brief Interest applied once per wave on unspent credits, in percent.
 */
constexpr int INTEREST_PERCENT = 5;