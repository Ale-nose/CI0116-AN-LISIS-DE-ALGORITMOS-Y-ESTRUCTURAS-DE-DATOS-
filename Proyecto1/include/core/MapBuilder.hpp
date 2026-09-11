// Copyright 2026 Kevin Velásquez García
#pragma once

#include "Grid.hpp"

#include <vector>
#include <utility>

using Point = std::pair<int, int>;

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

  // Fixed tower slot positions (level design, adjust freely).
  std::vector<Point> towerSlots = {
    {9, 0}, {9, 2},
    {1, 4}, {17, 4},
    {10, 4}, {10, 6},
    {5, 8}, {15, 8},
  };

  for (auto& [tx, ty] : towerSlots) {
    grid.set(tx, ty, CellType::TowerSlot);
  }
}