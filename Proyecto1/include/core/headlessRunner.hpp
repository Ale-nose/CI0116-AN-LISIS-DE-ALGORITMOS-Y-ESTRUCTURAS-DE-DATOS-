// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "Cliargs.hpp"

// Section 4.2 — runs a full match with no window, at maximum real-world
// speed, and writes the resulting Stats to a CSV file. Returns the
// process exit code.
int runHeadless(const CliArgs& args);
