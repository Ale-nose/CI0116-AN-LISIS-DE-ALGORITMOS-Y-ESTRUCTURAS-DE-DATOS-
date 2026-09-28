// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "CorePrices.hpp"

struct PlayerDecision {
  int wave = 1;
  int slot = 0;
  CoreType core = CoreType::LinkedList;
};

struct ReplayData {
  std::uint32_t seed = 0;
  std::vector<PlayerDecision> decisions;
};

bool saveReplay(const std::string& path, const ReplayData& replay);

bool loadReplay(const std::string& path, ReplayData& replay);
