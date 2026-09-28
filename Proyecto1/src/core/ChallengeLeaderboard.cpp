// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "ChallengeLeaderboard.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {

bool parseInteger(const std::string& text, int& value) {
  std::stringstream stream(text);
  stream >> value;

  return stream && stream.eof();
}

}  // namespace

ChallengeLeaderboard::ChallengeLeaderboard(
  const std::string& filePath)
  : filePath_(filePath) {
}

bool ChallengeLeaderboard::load() {
  results_.clear();

  std::ifstream file(filePath_);

  if (!file.is_open()) {
    return false;
  }

  std::string line;
  std::getline(file, line);

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::stringstream stream(line);
    ChallengeResult result;
    std::string waves;
    std::string lives;
    std::string credits;

    if (!std::getline(stream, result.player, ',')
      || !std::getline(stream, waves, ',')
      || !std::getline(stream, lives, ',')
      || !std::getline(stream, credits)) {
      continue;
    }

    if (result.player.empty()
      || !parseInteger(waves, result.wavesCompleted)
      || !parseInteger(lives, result.livesRemaining)
      || !parseInteger(credits, result.creditsRemaining)) {
      continue;
    }

    results_.push_back(result);
  }

  sortResults();
  return true;
}

bool ChallengeLeaderboard::save() const {
  std::ofstream file(filePath_);

  if (!file.is_open()) {
    return false;
  }

  file << "player,waves,lives,credits\n";

  for (const ChallengeResult& result : results_) {
    file << result.player << ','
         << result.wavesCompleted << ','
         << result.livesRemaining << ','
         << result.creditsRemaining << '\n';
  }

  return file.good();
}

void ChallengeLeaderboard::addResult(
  const ChallengeResult& result) {
  results_.push_back(result);
  sortResults();
}

const std::vector<ChallengeResult>& ChallengeLeaderboard::results() const {
  return results_;
}

void ChallengeLeaderboard::sortResults() {
  std::sort(results_.begin(), results_.end(), isBetterChallengeResult);
}
