// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

// Enemy movement and tower combat balance knobs (sections 2.1, 3.4).
// Starting values chosen so a full match plays out; tune them as a team.

/// Ticks an enemy needs to advance one path cell (6 -> 10 cells/second).
constexpr int ENEMY_TICKS_PER_CELL = 6;

/// A slot sees an enemy when both are at most this many cells apart
/// horizontally and vertically (square radius around the slot).
constexpr int TOWER_RANGE_CELLS = 2;

/// Life removed from an enemy by one effective tower shot.
constexpr int TOWER_DAMAGE = 40;