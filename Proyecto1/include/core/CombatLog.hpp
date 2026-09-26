// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Tower.hpp"

// Section 6.1 — field delimiter for the combat log. A comma loads
// directly in Google Sheets, LibreOffice and pandas; switch it to ';'
// if the report is opened in an Excel configured for a locale that
// uses ',' as the decimal separator.
constexpr char kLogDelimiter = ',';

// Decimal places used for the averaged columns (sizes, queue, time).
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
 * @brief Writes the combat log as a delimited text file with a header
 * row and one row per tower per wave. The seed is repeated on every
 * row so the logs of several runs can be concatenated as-is (Topic 7).
 * @param records Rows to write, in the order they were produced.
 * @param seed Seed of the match that produced them.
 * @param outPath Destination file path.
 * @return true if the file was written.
 */
bool writeCombatLog(const std::vector<CombatLogRecord>& records,
  std::uint32_t seed, const std::string& outPath);