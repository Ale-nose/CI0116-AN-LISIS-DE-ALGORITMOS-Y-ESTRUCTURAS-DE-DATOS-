// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Tower.hpp"

/**
 * @brief Section 6.1 — field delimiter for the combat log. A comma loads
 * directly in Google Sheets, LibreOffice and pandas; switch it to ';'
 * if the report is opened in an Excel configured for a locale that uses
 * ',' as the decimal separator.
 */
constexpr char kLogDelimiter = ',';

/**
 * @brief Decimal places used for the averaged columns (sizes, queue,
 * time).
 */
constexpr int kLogDecimals = 3;

/**
 * @brief One combat log row: one tower during one wave (section 7).
 */
struct CombatLogRecord {
  int wave = 0;              ///< 1-based wave number.
  int slot = 0;              ///< Slot index, 0..SLOTS-1.
  std::string structure;     ///< Installed core, as its CLI name.
  TowerWaveStats stats;      ///< Everything the tower measured itself.
  int effectiveShots = 0;    ///< Shots at an enemy that was still alive.
  int ghostShots = 0;        ///< Shots at an enemy that was already dead.
};

/**
 * @brief Match-wide settings repeated on every row, so the logs of many
 * runs (Topic 7) can be concatenated as-is and still be told apart.
 */
struct RunInfo {
  std::uint32_t seed = 0;         ///< Seed of the match.
  std::string hash = "default";   ///< Hash function of hash-table cores.
  std::string enemies = "all";    ///< "all", or the only category spawned.
};

/**
 * @brief Writes the combat log as a delimited text file with a header
 * row and one row per tower per wave.
 * @param records Rows to write, in the order they were produced.
 * @param run Match-wide settings repeated on every row.
 * @param outPath Destination file path.
 * @return true if the file was written.
 */
bool writeCombatLog(const std::vector<CombatLogRecord>& records,
  const RunInfo& run, const std::string& outPath);

/**
 * @brief Report question 5 — writes the bucket-length distribution of
 * every hash-table core, taken at its peak size in each wave: one row
 * per (wave, slot, chain length) with how many buckets had that length.
 * Records of non-hash cores are skipped.
 * @param records Combat log records holding each tower's peak buckets.
 * @param run Match-wide settings repeated on every row.
 * @param outPath Destination file path.
 * @return true if the file was written.
 */
bool writeBucketLog(const std::vector<CombatLogRecord>& records,
  const RunInfo& run, const std::string& outPath);