// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Wave.hpp"
#include <cmath>
#include <utility>
#include "EnemyIdFactory.hpp"

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

WaveComposition buildComposition(int totalSize,
    std::optional<EnemyCategory> onlyCategory) {
  WaveComposition composition;
  if (onlyCategory) {
    composition.perCategory[static_cast<std::size_t>(*onlyCategory)] =
      totalSize;
    return composition;
  }
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

WaveManager::WaveManager(std::uint32_t seed,
  std::optional<EnemyCategory> onlyCategory)
  : currentWave_(1),
    phase_(WavePhase::Construction),
    composition_(buildComposition(waveSize(1), onlyCategory)),
    nextToSpawn(0),
    combatTicks(0),
    spawnWindowTicks(0),
    totalSpawnedCount(0),
    wavesComplete_(false),
    onlyCategory_(onlyCategory),
    rng_(seed) {
}

void WaveManager::startCombat() {
  phase_ = WavePhase::Combat;
  spawnQueue.clear();
  for (std::size_t category = 0; category < kCategoryCount; ++category) {
    int count = composition_.perCategory[category];
    for (int i = 0; i < count; ++i) {
      spawnQueue.push_back(static_cast<EnemyCategory>(category));
    }
  }
  // Shuffle the spawn order with the seeded RNG.
  for (std::size_t i = spawnQueue.size(); i > 1; --i) {
    std::size_t j = static_cast<std::size_t>(rng_() % i);
    std::swap(spawnQueue[i - 1], spawnQueue[j]);
  }

  nextToSpawn = 0;
  combatTicks = 0;

  // Normal pace (one every WAVE_SPAWN_INTERVAL_TICKS) unless that would
  // take longer than WAVE_MAX_SPAWN_TICKS; then squeeze into that window.
  long long normalWindow =
    static_cast<long long>(spawnQueue.size()) * WAVE_SPAWN_INTERVAL_TICKS;
  spawnWindowTicks = static_cast<int>(
    normalWindow < WAVE_MAX_SPAWN_TICKS ? normalWindow
    : WAVE_MAX_SPAWN_TICKS);
}

std::vector<Enemy> WaveManager::tick(int initialDistance,
  std::size_t hiveIdStride) {
  std::vector<Enemy> spawned;
  if (phase_ != WavePhase::Combat || doneSpawning()) {
    return spawned;
  }

  ++combatTicks;
  // Integer spread: after t ticks, floor(t * size / window) enemies are
  // due. At the normal pace this is exactly one every
  // WAVE_SPAWN_INTERVAL_TICKS; for big waves several per tick.
  std::size_t total = spawnQueue.size();
  std::size_t due = static_cast<std::size_t>(
    static_cast<long long>(combatTicks) * static_cast<long long>(total)
    / spawnWindowTicks);
  if (due > total) {
    due = total;
  }

  while (nextToSpawn < due) {
    EnemyCategory category = spawnQueue[nextToSpawn];
    ++nextToSpawn;
    EnemyId id = generateEnemyId(category, totalSpawnedCount, hiveIdStride);
    ++totalSpawnedCount;
    spawned.emplace_back(id, category, initialDistance);
  }
  return spawned;
}

bool WaveManager::doneSpawning() const {
  return nextToSpawn >= spawnQueue.size();
}

void WaveManager::advanceToNextWave() {
  if (currentWave_ >= TOTAL_WAVES) {
    wavesComplete_ = true;
    return;
  }

  ++currentWave_;
  phase_ = WavePhase::Construction;
  composition_ = buildComposition(waveSize(currentWave_), onlyCategory_);
}
