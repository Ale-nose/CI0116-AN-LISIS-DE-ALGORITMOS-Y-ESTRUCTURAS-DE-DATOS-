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
  /**
   * @brief Orders the results best first (see isBetterChallengeResult()).
   */
  void sortResults();

  std::string filePath_;                  ///< CSV file backing the board.
  std::vector<ChallengeResult> results_;  ///< Results, best first.

 public:
  /**
   * @brief Creates an empty leaderboard backed by a CSV file.
   * @param filePath File to load from and save to.
   */
  explicit ChallengeLeaderboard(const std::string& filePath);

  /**
   * @brief Replaces the results with the ones stored in the file.
   * @return false if the file can't be opened (the board stays empty);
   * true otherwise.
   */
  bool load();

  /**
   * @brief Writes every result to the file, best first.
   * @return true if the file was written.
   */
  bool save() const;

  /**
   * @brief Adds a result and keeps the board ordered.
   * @param result Final result of a Challenge Mode match.
   */
  void addResult(const ChallengeResult& result);

  /**
   * @brief Every result, best first.
   * @return The ordered results.
   */
  const std::vector<ChallengeResult>& results() const;
};