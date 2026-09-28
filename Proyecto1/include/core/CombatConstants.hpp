// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "Ticks.hpp"  // for TICKS_PER_SECOND

/**
 * @file CombatConstants.hpp
 * @brief Enemy movement and tower combat balance knobs (sections 2.1,
 * 3.4). Starting values chosen so a full match plays out; tune them as a
 * team.
 */

/**
 * @brief Ticks an enemy needs to advance one path cell (6 -> 10
 * cells/second).
 */
constexpr int ENEMY_TICKS_PER_CELL = 6;

/**
 * @brief A slot sees an enemy when both are at most this many cells
 * apart horizontally and vertically (square radius around the slot).
 */
constexpr int TOWER_RANGE_CELLS = 2;

/**
 * @brief Life removed from an enemy by one effective tower shot.
 */
constexpr int TOWER_DAMAGE = 40;

/**
 * @brief Credits the Hollow Purple special ability costs per use.
 */
constexpr int HOLLOW_PURPLE_PRICE = 100000;

/**
 * @brief Ticks between paying for Hollow Purple and the moment it erases
 * every enemy on the map (18 seconds of game time). Matches the length of
 * assets/sounds/hollow_purple.wav, which plays during the charge: change
 * both together.
 */
constexpr int HOLLOW_PURPLE_CHARGE_TICKS = 18 * TICKS_PER_SECOND;

/**
 * @brief Ticks Hollow Purple needs after firing before it can be bought
 * again (90 seconds of game time, about a third of a full match).
 */
constexpr int HOLLOW_PURPLE_COOLDOWN_TICKS = 90 * TICKS_PER_SECOND;