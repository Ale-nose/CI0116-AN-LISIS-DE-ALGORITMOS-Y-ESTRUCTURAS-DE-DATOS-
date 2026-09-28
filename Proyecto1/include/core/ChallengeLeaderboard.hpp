// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <string>
#include <vector>
#include "ChallengeMode.hpp"

/**
 * @brief Stores and orders Challenge Mode results.
 */
class ChallengeLeaderboard {
 private:
  void sortResults();

  std::string filePath_;
  std::vector<ChallengeResult> results_;

 public:
  explicit ChallengeLeaderboard(const std::string& filePath);

  bool load();
  bool save() const;

  void addResult(const ChallengeResult& result);

  const std::vector<ChallengeResult>& results() const;
};
