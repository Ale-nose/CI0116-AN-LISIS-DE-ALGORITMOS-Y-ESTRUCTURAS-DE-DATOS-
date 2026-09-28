#include "SpatialGridIndex.hpp"

#include <algorithm>
#include <cmath>

SpatialGridIndex::SpatialGridIndex(int cellSize)
  : cellSize_(cellSize) {
}

void SpatialGridIndex::insert(EnemyId id, int x, int y) {
  positions_[id] = {x, y};
  cells_[cellOf(x, y)].push_back(id);
}

void SpatialGridIndex::erase(EnemyId id) {
  auto found = positions_.find(id);
  if (found == positions_.end()) {
    return;
  }
  positions_.erase(found);
  ++tombstones_;

  if (tombstones_ > static_cast<int>(positions_.size()) / 2 + 4) {
    rebuild();
  }
}

void SpatialGridIndex::rebuild() {
  cells_.clear();
  for (const auto& [id, point] : positions_) {
    cells_[cellOf(point.x, point.y)].push_back(id);
  }
  tombstones_ = 0;
}

std::optional<EnemyId> SpatialGridIndex::nearestNeighbor(int x, int y) const {
  if (positions_.empty()) {
    return std::nullopt;
  }

  CellKey center = cellOf(x, y);
  std::optional<EnemyId> best;
  double bestDistSq = -1.0;

  for (int radius = 0; radius <= 2 || !best; ++radius) {
    for (int dx = -radius; dx <= radius; ++dx) {
      for (int dy = -radius; dy <= radius; ++dy) {
        if (std::max(std::abs(dx), std::abs(dy)) != radius) {
          continue;
        }

        auto it = cells_.find({center.cx + dx, center.cy + dy});
        if (it == cells_.end()) {
          continue;
        }

        for (EnemyId id : it->second) {
          auto pos = positions_.find(id);
          if (pos == positions_.end()) {
            continue;  // stale (already erased)
          }

          double ddx = pos->second.x - x;
          double ddy = pos->second.y - y;
          double distSq = ddx * ddx + ddy * ddy;

          if (!best || distSq < bestDistSq) {
            best = id;
            bestDistSq = distSq;
          }
        }
      }
    }
    if (radius > 50) break;  // safety cutoff
  }
  return best;
}
