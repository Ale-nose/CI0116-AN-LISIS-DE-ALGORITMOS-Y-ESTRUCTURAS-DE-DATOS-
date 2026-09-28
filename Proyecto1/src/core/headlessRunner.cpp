// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "headlessRunner.hpp"

#include <iostream>
#include "CombatLog.hpp"
#include "CoreFactory.hpp"
#include "Simulation.hpp"

namespace {

/**
 * @brief Section 4.2 — prints a one-line match summary on stdout. The
 * detailed per-tower per-wave data goes to the combat log (--out).
 * @param stats Final statistics of the match.
 */
void printSummary(const Stats& stats) {
  std::cout << "ticks_run=" << stats.ticks_run
            << " simulated_ms=" << stats.simulated_ms
            << " shots_fired=" << stats.shots_fired
            << " total_steps=" << stats.total_steps
            << " waves_completed=" << stats.waves_completed << "\n";
}

}  // namespace

int runHeadless(const CliArgs& args) {
  if (args.core.empty()) {
    std::cerr << "--core <nombre> is required in headless mode\n";
    return 1;
  }

  std::optional<CoreType> coreType = coreTypeFromName(args.core);
  if (!coreType) {
    std::cerr << "Unknown --core name: " << args.core << "\n";
    return 1;
  }

  if (args.waves < 1 || args.waves > TOTAL_WAVES) {
    std::cerr << "--waves must be between 1 and " << TOTAL_WAVES << "\n";
    return 1;
  }

  std::optional<HashMode> hashMode = hashModeFromName(args.hash);
  if (!hashMode) {
    std::cerr << "Unknown --hash name: " << args.hash
              << " (use default or mixed)\n";
    return 1;
  }

  Config cfg;
  cfg.maxWaves = args.waves;
  cfg.hashMode = *hashMode;
  cfg.ignoreDefeat = args.ignoreDefeat;
  if (!args.only.empty()) {
    cfg.onlyCategory = categoryFromName(args.only);
    if (!cfg.onlyCategory) {
      std::cerr << "Unknown --only category: " << args.only << "\n";
      return 1;
    }
  }

  Simulation sim(args.seed, cfg);
  sim.installCoreEverywhere(*coreType);

  // Headless mode has no player to press "start wave", so it ends every
  // construction phase itself, right away: each wave starts on the same
  // tick the previous one ended.
  while (!sim.over()) {
    if (sim.inConstruction()) {
      sim.startWave();
    }
    sim.tick();
  }

  printSummary(sim.stats());

  RunInfo run;
  run.seed = args.seed;
  run.hash = args.hash;
  run.enemies = args.only.empty() ? "all" : args.only;
  if (!writeCombatLog(sim.combatLog(), run, args.outPath)) {
    return 1;
  }
  if (!args.bucketsOut.empty()
      && !writeBucketLog(sim.combatLog(), run, args.bucketsOut)) {
    return 1;
  }

  return 0;
}
