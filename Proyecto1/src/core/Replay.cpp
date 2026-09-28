// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez

#include "Replay.hpp"

#include <fstream>
#include <limits>
#include <sstream>

#include "CoreFactory.hpp"
#include "Slots.hpp"
#include "Wave.hpp"

namespace {

bool parseInt(const std::string& text, int& value) {
  std::stringstream stream(text);
  stream >> value;

  return stream && stream.eof();
}

bool parseSeed(
    const std::string& text,
    std::uint32_t& value) {
  unsigned long long parsed = 0;
  std::stringstream stream(text);
  stream >> parsed;

  if (!stream || !stream.eof()) {
    return false;
  }

  if (parsed > std::numeric_limits<std::uint32_t>::max()) {
    return false;
  }

  value = static_cast<std::uint32_t>(parsed);
  return true;
}

}  // namespace

bool saveReplay(
  const std::string& path,
  const ReplayData& replay) {
  std::ofstream file(path);

  if (!file.is_open()) {
    return false;
  }

  file << "seed," << replay.seed << '\n';
  file << "wave,slot,core\n";

  for (const PlayerDecision& decision : replay.decisions) {
    file << decision.wave << ','
         << decision.slot << ','
         << coreTypeToName(decision.core) << '\n';
  }

  return file.good();
}

bool loadReplay(
  const std::string& path,
  ReplayData& replay) {
  std::ifstream file(path);

  if (!file.is_open()) {
    return false;
  }

  ReplayData loaded;
  std::string line;

  if (!std::getline(file, line)) {
    return false;
  }

  std::stringstream seedLine(line);
  std::string key;
  std::string value;

  if (!std::getline(seedLine, key, ',')
    || !std::getline(seedLine, value)
    || key != "seed"
    || !parseSeed(value, loaded.seed)) {
    return false;
  }

  if (!std::getline(file, line)
    || line != "wave,slot,core") {
    return false;
  }

  int previousWave = 1;

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::stringstream stream(line);
    std::string waveText;
    std::string slotText;
    std::string coreText;

    if (!std::getline(stream, waveText, ',')
      || !std::getline(stream, slotText, ',')
      || !std::getline(stream, coreText)) {
      return false;
    }

    PlayerDecision decision;

    if (!parseInt(waveText, decision.wave)
      || !parseInt(slotText, decision.slot)) {
      return false;
    }

    if (decision.wave < 1 || decision.wave > TOTAL_WAVES
      || decision.wave < previousWave) {
      return false;
    }

    if (decision.slot < 0 || decision.slot >= SlotManager::kSlotCount) {
      return false;
    }

    const auto core = coreTypeFromName(coreText);

    if (!core) {
      return false;
    }

    decision.core = *core;
    loaded.decisions.push_back(decision);
    previousWave = decision.wave;
  }

  replay = loaded;
  return true;
}
