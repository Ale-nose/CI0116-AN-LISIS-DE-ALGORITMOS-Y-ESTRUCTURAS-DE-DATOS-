#include "headlessRunner.hpp"

#include <fstream>
#include <iostream>
#include "CoreFactory.hpp"
#include "Simulation.hpp"

namespace
{

void writeStats(const Stats& stats, const std::string& outPath)
{
    std::ofstream out(outPath);
    if (!out)
    {
        std::cerr << "Could not open output file: " << outPath << "\n";
        return;
    }

    out << "ticks_run,simulated_ms,shots_fired,total_steps,waves_completed\n";
    out << stats.ticks_run << "," << stats.simulated_ms << ","
        << stats.shots_fired << "," << stats.total_steps << ","
        << stats.waves_completed << "\n";
}

}  // namespace

int runHeadless(const CliArgs& args)
{
    if (args.core.empty())
    {
        std::cerr << "--core <nombre> is required in headless mode\n";
        return 1;
    }

    std::optional<CoreType> coreType = coreTypeFromName(args.core);
    if (!coreType)
    {
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

    while (!sim.over())
    {
        sim.tick();
    }

    writeStats(sim.stats(), args.outPath);

    return 0;
}