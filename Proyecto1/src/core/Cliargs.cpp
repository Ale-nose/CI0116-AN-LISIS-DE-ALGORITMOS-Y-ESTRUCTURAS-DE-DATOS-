// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Cliargs.hpp"

#include <cstdlib>
#include <iostream>

namespace {

bool needsValue(const char* flag, int i, int argc) {
  if (i + 1 >= argc) {
    std::cerr << "Missing value for " << flag << "\n";
    return false;
  }
  return true;
}

}  // namespace

bool parseCliArgs(int argc, char** argv, CliArgs& outArgs) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--headless") {
      outArgs.headless = true;
    } else if (arg == "--seed") {
      if (!needsValue("--seed", i, argc)) {
        return false;
      }
      outArgs.seed =
      static_cast<std::uint32_t>(std::strtoul(argv[++i], nullptr, 10));
    } else if (arg == "--waves") {
      if (!needsValue("--waves", i, argc)) {
        return false;
      }
      outArgs.waves = std::atoi(argv[++i]);
    } else if (arg == "--core") {
      if (!needsValue("--core", i, argc)) {
        return false;
      }
      outArgs.core = argv[++i];
    } else if (arg == "--out") {
      if (!needsValue("--out", i, argc)) {
        return false;
      }
      outArgs.outPath = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      return false;
    }
  }

  return true;
}
