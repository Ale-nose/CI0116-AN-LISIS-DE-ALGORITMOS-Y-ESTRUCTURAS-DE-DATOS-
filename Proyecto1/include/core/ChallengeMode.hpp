// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Fixed seed used by every Challenge Mode match.
 */
constexpr std::uint32_t CHALLENGE_SEED = 20260927;

/**
 * @brief Final result of one Challenge Mode match.
 */
struct ChallengeResult {
  std::string player;
  int wavesCompleted = 0;
  int livesRemaining = 0;
  int creditsRemaining = 0;
};

/**
 * @brief Returns true if first should rank above second.
 */
bool isBetterChallengeResult(
  const ChallengeResult& first,
  const ChallengeResult& second
);
