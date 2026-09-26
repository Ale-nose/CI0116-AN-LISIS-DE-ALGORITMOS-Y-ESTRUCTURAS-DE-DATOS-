// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <memory>
#include <optional>
#include <string>
#include "CorePrices.hpp"
#include "EnemyCategory.hpp"
#include "HashTableRegistry.hpp"
#include "TargetRegistry.hpp"

/**
 * @brief Sections 2.2 / 4.2 — builds a concrete ITargetRegistry, so
 * Simulation and the headless CLI can pick a structure without knowing
 * any of the 8 concrete classes.
 * @param type Structure to build.
 * @param hashMode Bucket function; only used for CoreType::HashTable.
 * @return The new, empty registry.
 */
std::unique_ptr<ITargetRegistry> createCore(CoreType type,
  HashMode hashMode = HashMode::Default);

/**
 * @brief Maps a CLI name (e.g. "avl", "hash_table") to its CoreType.
 * @param name Name given to --core.
 * @return The CoreType, or std::nullopt if the name isn't recognized.
 */
std::optional<CoreType> coreTypeFromName(const std::string& name);

/**
 * @brief Inverse of coreTypeFromName(): the CLI name of a CoreType, used
 * as the structure column of the combat log (section 6.1).
 * @param type Structure to name.
 * @return Its CLI name.
 */
std::string coreTypeToName(CoreType type);

/**
 * @brief Maps a CLI name ("default", "mixed") to its HashMode.
 * @param name Name given to --hash.
 * @return The HashMode, or std::nullopt if the name isn't recognized.
 */
std::optional<HashMode> hashModeFromName(const std::string& name);

/**
 * @brief Inverse of hashModeFromName(), for the combat log's hash column.
 * @param mode Hash function to name.
 * @return Its CLI name.
 */
std::string hashModeToName(HashMode mode);

/**
 * @brief Maps a CLI name ("swarm", "wraith", "hive", "decoy",
 * "colossus") to its EnemyCategory.
 * @param name Name given to --only.
 * @return The category, or std::nullopt if the name isn't recognized.
 */
std::optional<EnemyCategory> categoryFromName(const std::string& name);

/**
 * @brief Inverse of categoryFromName(), for the combat log's enemies
 * column.
 * @param category Category to name.
 * @return Its CLI name.
 */
std::string categoryToName(EnemyCategory category);