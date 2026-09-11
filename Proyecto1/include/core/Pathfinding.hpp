// Copyright 2026 Kevin Velásquez García
#pragma once

#include "Grid.hpp"
#include "Graph.hpp"
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

// What the rest of the game needs from the map, computed once.
struct RouteData {
  std::vector<Point> path;        // path[0] = entrance, path.back() = exit
  std::vector<Point> towerSlots;

  Point at(size_t index) const { return path[index]; }
  size_t length() const { return path.size(); }
};

// BFS from start to goal. All edges cost the same, so BFS already
// gives the shortest path (no need for Dijkstra).
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

// Collects every cell marked as a tower slot.
inline std::vector<Point> findTowerSlots(const Grid& grid) {
  std::vector<Point> slots;
  for (int y = 0; y < GRID_HEIGHT; ++y) {
    for (int x = 0; x < GRID_WIDTH; ++x) {
      if (grid.at(x, y) == CellType::TowerSlot) {
        slots.push_back({x, y});
      }
    }
  }
  return slots;
}

// Computes the route and tower slots once, at game start.
inline RouteData buildRoute(const Grid& grid, Point entrance, Point exit) {
  Graph graph = buildGraphFromGrid(grid);

  RouteData route;
  route.path = shortestPath(graph, entrance, exit);
  route.towerSlots = findTowerSlots(grid);
  return route;
}
