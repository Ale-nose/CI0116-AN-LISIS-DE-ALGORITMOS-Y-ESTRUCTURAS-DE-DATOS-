// Copyright 2026 Kevin Velásquez García
#pragma once

#include "Grid.hpp"

#include <vector>
#include <utility>

using Point = std::pair<int, int>;

// Fixed tower slot positions (level design). Order defines slot index:
// slot 0 is towerSlotPositions()[0], slot 1 is [1], etc. — SlotManager
// uses this same 0..7 indexing, so MapView can map a click on one of
// these cells directly to the slot index Simulation/SlotManager expects.
inline const std::vector<Point>& towerSlotPositions() {
  static const std::vector<Point> kPositions = {
    {9, 0}, {9, 2},
    {1, 4}, {17, 4},
    {10, 4}, {10, 6},
    {5, 8}, {15, 8},
  };
  return kPositions;
}

// Paints a straight (horizontal or vertical) run of Path cells between
// two points, both ends included.
inline void fillPathSegment(Grid& grid, Point from, Point to) {
  int x = from.first, y = from.second;
  int dx =
    (to.first  > from.first)  ? 1 : (to.first  < from.first)  ? -1 : 0;
  int dy =
    (to.second > from.second) ? 1 : (to.second < from.second) ? -1 : 0;

  while (true) {
    grid.set(x, y, CellType::Path);
    if (x == to.first && y == to.second) break;
    x += dx;
    y += dy;
  }
}

// Sample fixed map: a serpentine path plus 8 tower slots.
inline void buildDefaultMap(Grid& grid) {
  // Corners of the path; segments between them are filled automatically.
  std::vector<Point> waypoints = {
    {0, 1}, {18, 1},
    {18, 3}, {2, 3},
    {2, 5}, {18, 5},
    {18, 7}, {2, 7},
    {2, 9}, {19, 9},
  };

  for (size_t i = 0; i + 1 < waypoints.size(); ++i) {
    fillPathSegment(grid, waypoints[i], waypoints[i + 1]);
  }

  grid.set(0, 1, CellType::Entrance);
  grid.set(19, 9, CellType::Exit);

  // Fixed tower slot positions, shared with SlotManager via
  // towerSlotPositions() so a click on cell i always means slot i.
  for (auto& [tx, ty] : towerSlotPositions()) {
    grid.set(tx, ty, CellType::TowerSlot);
  }
}