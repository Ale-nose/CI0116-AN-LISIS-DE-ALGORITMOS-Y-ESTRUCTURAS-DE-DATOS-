// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "headlessRunner.hpp"

#include <iostream>
#include "CombatLog.hpp"
#include "CoreFactory.hpp"
#include "Simulation.hpp"

namespace {

// Section 4.2 — one-line match summary on stdout. The detailed per-tower
// per-wave data goes to the combat log file (--out), section 6.1.
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

  // ### args.waves is accepted by the CLI but Simulation always plays
  // ### out all TOTAL_WAVES (Wave.hpp) — capping the match short at
  // ### args.waves needs a stop-early hook in WaveManager/Simulation
  // ### that doesn't exist yet.
  Config cfg;
  Simulation sim(args.seed, cfg);
  sim.installCoreEverywhere(*coreType);

  while (!sim.over()) {
    sim.tick();
  }

  printSummary(sim.stats());
  if (!writeCombatLog(sim.combatLog(), args.seed, args.outPath)) {
    return 1;
  }

  return 0;
}