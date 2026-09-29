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
  std::string player;        ///< Name the player entered.
  int wavesCompleted = 0;    ///< Waves survived (first ranking key).
  int livesRemaining = 0;    ///< Lives left (second key).
  int creditsRemaining = 0;  ///< Credits left (third key).
};

/**
 * @brief Ranking order: more waves completed first, then more lives
 * remaining, then more credits remaining.
 * @param first A result.
 * @param second Another result.
 * @return true if first should rank above second.
 */
bool isBetterChallengeResult(
  const ChallengeResult& first,
  const ChallengeResult& second
);