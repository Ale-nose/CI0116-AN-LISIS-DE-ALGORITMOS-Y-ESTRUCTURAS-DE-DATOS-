// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <functional>
#include "Grid.hpp"

/**
 * @brief A cell position: (x, y) = (column, row).
 */
using Point = std::pair<int, int>;

/**
 * @brief Hash for Point, so it can be used as an unordered_map key.
 */
struct PointHash {
  /**
   * @brief Hashes a position.
   * @param p Position to hash.
   * @return Its hash value.
   */
  size_t operator()(const Point& p) const {
    return std::hash<int>()(p.first) * 31 + std::hash<int>()(p.second);
  }
};

/**
 * @brief Whether an enemy can stand on a cell.
 * @param t Cell content.
 * @return true for path, entrance and exit cells.
 */
inline bool isWalkable(CellType t) {
  return t == CellType::Path || t == CellType::Entrance || t == CellType::Exit;
}

/**
 * @brief The walkable part of the map as a graph.
 */
struct Graph {
  std::vector<Point> nodes;  ///< Every walkable cell.
  /// Walkable neighbors (up, down, left, right) of each node.
  std::unordered_map<Point, std::vector<Point>, PointHash> adjacency;
};

/**
 * @brief Builds the graph once from the grid's current layout.
 * @param grid The map.
 * @return Its walkable cells and their walkable neighbors.
 */
inline Graph buildGraphFromGrid(const Grid& grid) {
  Graph graph;

  // Pass 1: collect every walkable cell as a node.
  for (int y = 0; y < GRID_HEIGHT; ++y) {
    for (int x = 0; x < GRID_WIDTH; ++x) {
      if (isWalkable(grid.at(x, y))) {
        graph.nodes.push_back({x, y});
      }
    }
  }

  const std::vector<Point> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

  // Pass 2: for each node, keep only its walkable neighbors.
  for (const auto& node : graph.nodes) {
    int x = node.first, y = node.second;
    std::vector<Point> neighbors;
    for (const auto& dir : directions) {
      int nx = x + dir.first, ny = y + dir.second;
      if (Grid::inBounds(nx, ny) && isWalkable(grid.at(nx, ny))) {
        neighbors.push_back({nx, ny});
      }
    }
    graph.adjacency[node] = neighbors;
  }

  return graph;
}