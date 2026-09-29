// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include "Grid.hpp"
#include "Graph.hpp"

/**
 * @brief What the rest of the game needs from the map, computed once.
 */
struct RouteData {
  std::vector<Point> path;        ///< path[0] = entrance, back() = exit.
  std::vector<Point> towerSlots;  ///< Every tower slot cell.

  /**
   * @brief Cell at a position along the route.
   * @param index Steps from the entrance, 0..length()-1.
   * @return That cell.
   */
  Point at(size_t index) const {
    return path[index];
  }

  /**
   * @brief Number of cells from entrance to exit, both included.
   * @return The route length.
   */
  size_t length() const {
    return path.size();
  }
};

/**
 * @brief Shortest path by breadth-first search. All edges cost the same,
 * so BFS already gives the shortest path (no need for Dijkstra).
 * @param graph Walkable cells and their neighbors.
 * @param start First cell.
 * @param goal Last cell; must be reachable from start.
 * @return The cells from start to goal, both included.
 */
inline std::vector<Point> shortestPath(
  const Graph& graph, Point start, Point goal) {
  std::unordered_map<Point, Point, PointHash> cameFrom;
  std::unordered_map<Point, bool, PointHash> visited;

  std::queue<Point> frontier;
  frontier.push(start);
  visited[start] = true;

  while (!frontier.empty()) {
    Point current = frontier.front();
    frontier.pop();
    if (current == goal) break;

    for (const auto& next : graph.adjacency.at(current)) {
      if (!visited[next]) {
        visited[next] = true;
        cameFrom[next] = current;
        frontier.push(next);
      }
    }
  }

  // Walk cameFrom backwards from goal to start, then reverse.
  std::vector<Point> path;
  Point current = goal;
  path.push_back(current);
  while (current != start) {
    current = cameFrom.at(current);
    path.push_back(current);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

/**
 * @brief Collects every cell marked as a tower slot.
 * @note The local list is named towerCells, not slots: Qt defines `slots`
 * as a macro, which mangles a variable with that exact name in any file
 * that also includes Qt headers.
 * @param grid The map.
 * @return Tower slot cells, row by row.
 */
inline std::vector<Point> findTowerSlots(const Grid& grid) {
  std::vector<Point> towerCells;
  for (int y = 0; y < GRID_HEIGHT; ++y) {
    for (int x = 0; x < GRID_WIDTH; ++x) {
      if (grid.at(x, y) == CellType::TowerSlot) {
        towerCells.push_back({x, y});
      }
    }
  }
  return towerCells;
}

/**
 * @brief Computes the route and tower slots once, at game start.
 * @param grid The map.
 * @param entrance Cell where enemies appear.
 * @param exit The base.
 * @return The route and the tower slots.
 */
inline RouteData buildRoute(const Grid& grid, Point entrance, Point exit) {
  Graph graph = buildGraphFromGrid(grid);

  RouteData route;
  route.path = shortestPath(graph, entrance, exit);
  route.towerSlots = findTowerSlots(grid);
  return route;
}