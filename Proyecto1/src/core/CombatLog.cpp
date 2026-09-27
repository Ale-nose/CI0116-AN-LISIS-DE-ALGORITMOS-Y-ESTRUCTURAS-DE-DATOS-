// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "CombatLog.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>

namespace {

constexpr double kNanosecondsPerMicrosecond = 1000.0;

/**
 * @brief Mean of a per-tick sum.
 * @param sum Sum of one value sampled every tick.
 * @param ticks Number of ticks sampled.
 * @return sum / ticks, or 0 when the tower saw no ticks at all.
 */
double perTickMean(std::uint64_t sum, int ticks) {
  if (ticks == 0) {
    return 0.0;
  }
  return static_cast<double>(sum) / ticks;
}

/**
 * @brief Opens a log file with the classic locale, so decimals always
 * use '.' regardless of the user's locale.
 * @param out Stream to open.
 * @param outPath Destination file path.
 * @return false (after printing the reason to stderr) if it can't open.
 */
bool openLog(std::ofstream& out, const std::string& outPath) {
  out.open(outPath);
  if (!out) {
    std::cerr << "Could not open log file: " << outPath << "\n";
    return false;
  }
  out.imbue(std::locale::classic());
  out << std::fixed << std::setprecision(kLogDecimals);
  return true;
}

/**
 * @brief Writes the match-wide columns that start every row.
 * @param out Destination stream.
 * @param run Seed, hash function and enemy set of the match.
 */
void writeRunColumns(std::ostream& out, const RunInfo& run) {
  const char d = kLogDelimiter;
  out << run.seed << d << run.hash << d << run.enemies << d;
}

/**
 * @brief Writes the combat log's header row.
 * @param out Destination stream.
 */
void writeHeader(std::ostream& out) {
  const char d = kLogDelimiter;
  out << "seed" << d << "hash" << d << "enemies" << d
      << "wave" << d << "slot" << d << "structure" << d
      << "ticks" << d << "max_size" << d << "avg_size" << d
      << "inserts" << d << "erases" << d << "queries" << d
      << "steps_total" << d << "steps_insert" << d << "steps_erase" << d
      << "steps_query" << d << "comparisons" << d << "pointer_hops" << d
      << "shifts" << d << "rotations" << d << "real_us" << d
      << "max_queue" << d << "avg_queue" << d << "empty_queue_ticks" << d
      << "empty_queue_ratio" << d << "effective_shots" << d
      << "ghost_shots" << d << "peak_height" << d
      << "peak_bucket_count" << d << "peak_used_buckets" << d
      << "peak_max_bucket" << '\n';
}

/**
 * @brief Writes one combat log row: one tower during one wave.
 * @param out Destination stream.
 * @param r The tower's record for the wave.
 * @param run Match-wide settings for the leading columns.
 */
void writeRow(std::ostream& out, const CombatLogRecord& r,
    const RunInfo& run) {
  const char d = kLogDelimiter;
  const TowerWaveStats& s = r.stats;
  std::uint64_t stepsTotal = s.insertSteps + s.eraseSteps + s.querySteps;
  double emptyRatio = (s.ticks == 0)
    ? 0.0
    : static_cast<double>(s.emptyQueueTicks) / s.ticks;

  int usedBuckets = 0;
  int maxBucket = 0;
  for (int length : s.peakBuckets) {
    if (length > 0) {
      ++usedBuckets;
    }
    if (length > maxBucket) {
      maxBucket = length;
    }
  }

  writeRunColumns(out, run);
  out << r.wave << d << r.slot << d << r.structure << d
      << s.ticks << d << s.maxSize << d << perTickMean(s.sizeSum, s.ticks)
      << d << s.inserts << d << s.erases << d << s.queries << d
      << stepsTotal << d << s.insertSteps << d << s.eraseSteps << d
      << s.querySteps << d << s.breakdown.comparisons << d
      << s.breakdown.pointerHops << d << s.breakdown.shifts << d
      << s.breakdown.rotations << d
      << s.realNanoseconds / kNanosecondsPerMicrosecond << d
      << s.maxPending << d << perTickMean(s.pendingSum, s.ticks) << d
      << s.emptyQueueTicks << d << emptyRatio << d
      << r.effectiveShots << d << r.ghostShots << d
      << s.peakHeight << d << s.peakBuckets.size() << d
      << usedBuckets << d << maxBucket << '\n';
}

}  // namespace

bool writeCombatLog(const std::vector<CombatLogRecord>& records,
    const RunInfo& run, const std::string& outPath) {
  std::ofstream out;
  if (!openLog(out, outPath)) {
    return false;
  }
  writeHeader(out);
  for (const CombatLogRecord& record : records) {
    writeRow(out, record, run);
  }
  return static_cast<bool>(out);
}

bool writeBucketLog(const std::vector<CombatLogRecord>& records,
    const RunInfo& run, const std::string& outPath) {
  std::ofstream out;
  if (!openLog(out, outPath)) {
    return false;
  }
  const char d = kLogDelimiter;
  out << "seed" << d << "hash" << d << "enemies" << d << "wave" << d
      << "slot" << d << "size" << d << "chain_length" << d << "buckets"
      << '\n';

  for (const CombatLogRecord& record : records) {
    const std::vector<int>& chains = record.stats.peakBuckets;
    if (chains.empty()) {
      continue;  // not a hash-table core
    }
    std::map<int, int> histogram;  // ordered: rows come out by length
    for (int length : chains) {
      ++histogram[length];
    }
    for (const auto& [length, buckets] : histogram) {
      writeRunColumns(out, run);
      out << record.wave << d << record.slot << d << record.stats.maxSize
          << d << length << d << buckets << '\n';
    }
  }
  return static_cast<bool>(out);
}
