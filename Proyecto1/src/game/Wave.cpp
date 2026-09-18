// Copyright 2026 Kevin Velásquez García
#include "Wave.hpp"

#include <cmath>

int WaveComposition::total() const {
  int sum = 0;
  for (int count : perCategory) {
    sum += count;
  }
  return sum;
}

int waveSize(int waveNumber) {
  double size = WAVE_BASE * std::pow(WAVE_FACTOR, waveNumber - 1);
  return static_cast<int>(size + 0.5);  // round to nearest
}

WaveComposition buildComposition(int totalSize) {
  WaveComposition composition;
  int base = totalSize / static_cast<int>(kCategoryCount);
  int remainder = totalSize % static_cast<int>(kCategoryCount);

  for (std::size_t i = 0; i < kCategoryCount; ++i) {
    // Spread the remainder across the first few categories so the
    // total still adds up exactly to totalSize.
    composition.perCategory[i] = 
      base + (static_cast<int>(i) < remainder ? 1 : 0);
  }
  return composition;
}

WaveManager::WaveManager()
    : currentWave_(1),
      phase_(WavePhase::Construction),
      composition_(buildComposition(waveSize(1))),
      nextToSpawn(0),
      ticksSinceLastSpawn(0) {}

void WaveManager::startCombat() {
  phase_ = WavePhase::Combat;
  spawnQueue.clear();
  for (std::size_t category = 0; category < kCategoryCount; ++category) {
    int count = composition_.perCategory[category];
    for (int i = 0; i < count; ++i) {
      spawnQueue.push_back(static_cast<EnemyCategory>(category));
    }
  }
  nextToSpawn = 0;
  ticksSinceLastSpawn = 0;
}

std::optional<EnemyCategory> WaveManager::tick() {
  if (phase_ != WavePhase::Combat || doneSpawning()) {
    return std::nullopt;
  }

  ++ticksSinceLastSpawn;
  if (ticksSinceLastSpawn < WAVE_SPAWN_INTERVAL_TICKS) {
    return std::nullopt;
  }

  ticksSinceLastSpawn = 0;
  EnemyCategory category = spawnQueue[nextToSpawn];
  ++nextToSpawn;

  // TODO(Theme 3.1/3.4): this is where an actual Enemy should be created
  // (assigning it an EnemyId, placing it at RouteData::at(0), and giving
  // it whatever stats its `category` implies) and handed off to whatever
  // tracks live enemies. WaveManager only decides timing/category, since
  // it has no concept of Enemy, EnemyId, or the route.

  return category;
}

bool WaveManager::doneSpawning() const {
  return nextToSpawn >= spawnQueue.size();
}

void WaveManager::advanceToNextWave() {
  ++currentWave_;
  phase_ = WavePhase::Construction;
  if (!allWavesComplete()) {
    composition_ = buildComposition(waveSize(currentWave_));
  }
}
