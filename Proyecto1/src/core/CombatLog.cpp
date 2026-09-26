// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "CombatLog.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>

namespace {

constexpr double kNanosecondsPerMicrosecond = 1000.0;

// Mean of a per-tick sum; 0 when the tower saw no ticks at all.
double perTickMean(std::uint64_t sum, int ticks) {
  if (ticks == 0) {
    return 0.0;
  }
  return static_cast<double>(sum) / ticks;
}

void writeHeader(std::ostream& out) {
  const char d = kLogDelimiter;
  out << "seed" << d << "wave" << d << "slot" << d << "structure" << d
      << "ticks" << d << "max_size" << d << "avg_size" << d
      << "inserts" << d << "erases" << d << "queries" << d
      << "steps_total" << d << "steps_insert" << d << "steps_erase" << d
      << "steps_query" << d << "comparisons" << d << "pointer_hops" << d
      << "shifts" << d << "rotations" << d << "real_us" << d
      << "max_queue" << d << "avg_queue" << d << "empty_queue_ticks" << d
      << "empty_queue_ratio" << d << "effective_shots" << d
      << "ghost_shots" << '\n';
}

void writeRow(std::ostream& out, const CombatLogRecord& r,
    std::uint32_t seed) {
  const char d = kLogDelimiter;
  const TowerWaveStats& s = r.stats;
  std::uint64_t stepsTotal = s.insertSteps + s.eraseSteps + s.querySteps;
  double emptyRatio = (s.ticks == 0)
    ? 0.0
    : static_cast<double>(s.emptyQueueTicks) / s.ticks;

  out << seed << d << r.wave << d << r.slot << d << r.structure << d
      << s.ticks << d << s.maxSize << d << perTickMean(s.sizeSum, s.ticks)
      << d << s.inserts << d << s.erases << d << s.queries << d
      << stepsTotal << d << s.insertSteps << d << s.eraseSteps << d
      << s.querySteps << d << s.breakdown.comparisons << d
      << s.breakdown.pointerHops << d << s.breakdown.shifts << d
      << s.breakdown.rotations << d
      << s.realNanoseconds / kNanosecondsPerMicrosecond << d
      << s.maxPending << d << perTickMean(s.pendingSum, s.ticks) << d
      << s.emptyQueueTicks << d << emptyRatio << d
      << r.effectiveShots << d << r.ghostShots << '\n';
}

}  // namespace

bool writeCombatLog(const std::vector<CombatLogRecord>& records,
    std::uint32_t seed, const std::string& outPath) {
  std::ofstream out(outPath);
  if (!out) {
    std::cerr << "Could not open combat log file: " << outPath << "\n";
    return false;
  }

  // Force '.' as decimal separator regardless of the user's locale, so
  // the file always parses the same way.
  out.imbue(std::locale::classic());
  out << std::fixed << std::setprecision(kLogDecimals);

  writeHeader(out);
  for (const CombatLogRecord& record : records) {
    writeRow(out, record, seed);
  }
  return static_cast<bool>(out);
}