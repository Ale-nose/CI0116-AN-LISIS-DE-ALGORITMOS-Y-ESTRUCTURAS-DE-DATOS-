// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstdint>
#include <string>
#include "Wave.hpp"  // for TOTAL_WAVES

// Section 4.2 — command-line contract:
// overflow --headless --seed N --waves M --core <nombre> --out resultados.csv
struct CliArgs {
  bool headless = false;
  std::uint32_t seed = 0;
  int waves = TOTAL_WAVES;
  std::string core;
  std::string outPath = "resultados.csv";
};

// Parses argv into outArgs. Returns false (and prints an error to stderr)
// on a missing value or an unrecognized flag.
bool parseCliArgs(int argc, char** argv, CliArgs& outArgs);
