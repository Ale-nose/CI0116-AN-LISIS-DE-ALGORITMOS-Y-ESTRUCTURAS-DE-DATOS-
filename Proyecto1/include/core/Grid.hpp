// Copyright 2026 Kevin Velásquez García
#pragma once

#include <array>

const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 12;

// Possible contents of a single map cell.
enum class CellType {
  Empty,
  Path,
  Entrance,
  Exit,
  TowerSlot
};

class Grid {
 private:
  // Rows first (GRID_HEIGHT), columns second (GRID_WIDTH).
  std::array<std::array<CellType, GRID_WIDTH>, GRID_HEIGHT> cells;

 public:
  Grid() {
    for (auto& row : cells) {
      row.fill(CellType::Empty);
    }
  }

  CellType at(int x, int y) const {
    return cells[y][x];  // row = y, column = x
  }

  void set(int x, int y, CellType tipo) {
    cells[y][x] = tipo;  // row = y, column = x
  }

  // Checks whether (x, y) fits inside the grid.
  static bool inBounds(int x, int y) {
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
  }
};
