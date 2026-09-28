// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "ChallengeMode.hpp"

bool isBetterChallengeResult(
  const ChallengeResult& first,
  const ChallengeResult& second) {
  if (first.wavesCompleted != second.wavesCompleted) {
    return first.wavesCompleted > second.wavesCompleted;
  }

  if (first.livesRemaining != second.livesRemaining) {
    return first.livesRemaining > second.livesRemaining;
  }

  if (first.creditsRemaining != second.creditsRemaining) {
    return first.creditsRemaining > second.creditsRemaining;
  }

  return first.player < second.player;
}
