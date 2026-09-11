// Copyright 2026 Kevin Velásquez García
#pragma once

#include "Grid.hpp"
#include <vector>
#include <utility>
#include <unordered_map>
#include <functional>

using Point = std::pair<int, int>;

// Hash for Point so it can be used as an unordered_map key.
struct PointHash {
  size_t operator()(const Point& p) const {
      return std::hash<int>()(p.first) * 31 + std::hash<int>()(p.second);
  }
};

// A cell an enemy can actually stand on.
inline bool isWalkable(CellType t) {
    return t == CellType::Path || t == CellType::Entrance || t == CellType::Exit;
}

// nodes: every walkable cell. adjacency: walkable neighbors per cell.
struct Graph {
  std::vector<Point> nodes;
  std::unordered_map<Point, std::vector<Point>, PointHash> adjacency;
};

// Builds the graph once from the grid's current layout.
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