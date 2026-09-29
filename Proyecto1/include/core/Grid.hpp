// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <array>

/**
 * @brief Map width in cells (section 2.1).
 */
constexpr int GRID_WIDTH = 20;

/**
 * @brief Map height in cells (section 2.1).
 */
constexpr int GRID_HEIGHT = 12;

/**
 * @brief Possible contents of a single map cell.
 */
enum class CellType {
  Empty,      ///< Nothing: enemies can't walk it, towers can't go there.
  Path,       ///< Part of the enemies' route.
  Entrance,   ///< Where enemies appear.
  Exit,       ///< The base: an enemy reaching it costs a life.
  TowerSlot   ///< One of the 8 fixed tower slots.
};

/**
 * @brief The GRID_WIDTH x GRID_HEIGHT map, one CellType per cell.
 */
class Grid {
 private:
  /// Rows first (GRID_HEIGHT), columns second (GRID_WIDTH).
  std::array<std::array<CellType, GRID_WIDTH>, GRID_HEIGHT> cells;

 public:
  /**
   * @brief Creates a map with every cell empty.
   */
  Grid() {
    for (auto& row : cells) {
      row.fill(CellType::Empty);
    }
  }

  /**
   * @brief Content of a cell.
   * @param x Column, 0..GRID_WIDTH-1.
   * @param y Row, 0..GRID_HEIGHT-1.
   * @return What the cell holds.
   */
  CellType at(int x, int y) const {
    return cells[y][x];  // row = y, column = x
  }

  /**
   * @brief Changes the content of a cell.
   * @param x Column, 0..GRID_WIDTH-1.
   * @param y Row, 0..GRID_HEIGHT-1.
   * @param type What the cell will hold.
   */
  void set(int x, int y, CellType type) {
    cells[y][x] = type;  // row = y, column = x
  }

  /**
   * @brief Checks whether a position is inside the grid.
   * @param x Column.
   * @param y Row.
   * @return true if (x, y) is a valid cell.
   */
  static bool inBounds(int x, int y) {
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
  }
};