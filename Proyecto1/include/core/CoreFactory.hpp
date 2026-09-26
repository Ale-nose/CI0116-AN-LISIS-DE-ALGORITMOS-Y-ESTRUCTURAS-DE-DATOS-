// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <memory>
#include <optional>
#include <string>
#include "CorePrices.hpp"
#include "TargetRegistry.hpp"

// Section 2.2 / 4.2 — builds a concrete ITargetRegistry for a CoreType,
// so Simulation and the headless CLI's --core <nombre> can pick a
// structure without knowing about any of the 8 concrete classes.
std::unique_ptr<ITargetRegistry> createCore(CoreType type);

// Maps a CLI-friendly name (e.g. "avl", "hash_table") to its CoreType.
// Returns std::nullopt if the name isn't recognized.
std::optional<CoreType> coreTypeFromName(const std::string& name);

// Inverse of coreTypeFromName(): the CLI-friendly name of a CoreType,
// used as the structure column of the combat log (section 6.1).
std::string coreTypeToName(CoreType type);