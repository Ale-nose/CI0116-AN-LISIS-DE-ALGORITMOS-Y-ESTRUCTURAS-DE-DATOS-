// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include "Wave.hpp"  // for TOTAL_WAVES

/**
 * @brief Section 4.2 — parsed command line.
 *
 * Contract: overflow --headless --seed N --waves M --core <nombre>
 * --out resultados.csv. Extra options for the report experiments
 * (Topic 7): --hash, --only, --buckets-out and --ignore-defeat.
 */
struct CliArgs {
  bool headless = false;       ///< Run without a window (--headless).
  bool challenge = false;      ///< Use fixed Challenge Mode settings.
  std::uint32_t seed = 0;      ///< Match seed (--seed).
  int waves = TOTAL_WAVES;     ///< Last wave to play (--waves).
  std::string core;            ///< Structure for every slot (--core).
  std::string outPath = "resultados.csv";  ///< Combat log path (--out).
  std::string hash = "default";  ///< Hash function: default|mixed (--hash).
  std::string only;        ///< Only category to spawn; empty: all (--only).
  std::string bucketsOut;  ///< Bucket log path; empty: none (--buckets-out).
  bool ignoreDefeat = false;   ///< Keep playing at 0 lives (--ignore-defeat).
  std::string replayPath;
};

/**
 * @brief Parses the command line.
 * @param argc Argument count, as received by main().
 * @param argv Argument values, as received by main().
 * @param outArgs Filled with every option found.
 * @return false (after printing the reason to stderr) on a missing value
 * or an unrecognized flag; true otherwise.
 */
bool parseCliArgs(int argc, char** argv, CliArgs& outArgs);
