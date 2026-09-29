// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "CorePrices.hpp"

/**
 * @brief One tower purchase or replacement made by the player.
 */
struct PlayerDecision {
  int wave = 1;                          ///< Wave it was made before.
  int slot = 0;                          ///< Slot index, 0..SLOTS-1.
  CoreType core = CoreType::LinkedList;  ///< Structure installed.
};

/**
 * @brief Everything needed to replay a match: its seed plus the player's
 * decisions, in the order they were made.
 */
struct ReplayData {
  std::uint32_t seed = 0;                 ///< Match seed.
  std::vector<PlayerDecision> decisions;  ///< Purchases, by wave.
};

/**
 * @brief Writes a replay as CSV: a "seed,N" line, a "wave,slot,core"
 * header, then one line per decision.
 * @param path Destination file path.
 * @param replay Match to save.
 * @return true if the file was written.
 */
bool saveReplay(const std::string& path, const ReplayData& replay);

/**
 * @brief Reads a replay written by saveReplay().
 * @param path File to read.
 * @param replay Filled with the match on success; left untouched if the
 * file is missing or malformed.
 * @return true if the file was read and every line was valid.
 */
bool loadReplay(const std::string& path, ReplayData& replay);