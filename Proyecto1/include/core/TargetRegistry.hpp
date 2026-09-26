// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstddef>
#include <vector>
#include "KeyType.hpp"

/**
 * @brief Per-operation step counter, shared by every concrete registry.
 *
 * Every step an operation performs is recorded here exactly once, and
 * each interface method returns total() after the operation minus
 * total() before it — so the cost that blocks a tower, the log's
 * steps_total and the sum of the four categories are always the same
 * number. Category rules, applied identically by all 8 registries:
 *  - comparison(): examining one element/key/bucket (a three-way compare
 *    of the same pair counts once).
 *  - pointerHop(): following or rewriting a link to reach or attach an
 *    element: walking to the next node/child, splicing or attaching a
 *    node, jumping from a hash to its bucket, peeking a known slot.
 *  - shift(): writing an element into an array position: shifting,
 *    swapping, appending, or copying during grow/rehash.
 *  - rotation(): one tree rotation.
 */
struct StepCounter {
  int comparisons = 0;  ///< Steps spent comparing.
  int pointerHops  = 0;  ///< Steps spent following or rewriting links.
  int shifts       = 0;  ///< Steps spent writing into array positions.
  int rotations    = 0;  ///< Tree rotations.

  /**
   * @brief Steps recorded in every category so far.
   * @return Sum of the four categories.
   */
  int total() const {
    return comparisons + pointerHops + shifts + rotations;
  }

  /**
   * @brief Records one comparison step.
   */
  void comparison() {
    ++comparisons;
  }

  /**
   * @brief Records one pointer-hop step.
   */
  void pointerHop() {
    ++pointerHops;
  }

  /**
   * @brief Records one shift step.
   */
  void shift() {
    ++shifts;
  }

  /**
   * @brief Records one rotation step.
   */
  void rotation() {
    ++rotations;
  }
};

/**
 * @brief Snapshot of a registry's internal shape, for the report only
 * (Topic 7, questions 5 and 6). Taking it costs no steps and never
 * affects the game. Fields a structure doesn't have keep their defaults.
 */
struct RegistryShape {
  int height = -1;                  ///< Tree height (-1: not a tree).
  std::vector<int> bucketLengths;   ///< One entry per bucket (hash only).
};

/**
 * @brief Common interface for all 8 targeting-core implementations.
 *
 * Pure interface: every method is a contract, not an implementation.
 * Each of the 8 concrete registries inherits from it and fills in the
 * actual logic. Every operation returns its cost in steps (see
 * StepCounter for what counts as a step).
 */
class ITargetRegistry {
 public:
    virtual ~ITargetRegistry() = default;

    /**
     * @brief Inserts an enemy.
     * @param id Enemy identifier.
     * @param k Key the structure indexes it by (== id in this project).
     * @return Steps consumed.
     */
    virtual int insert(EnemyId id, Key k) = 0;

    /**
     * @brief Removes an enemy, if present.
     * @param id Enemy identifier.
     * @return Steps consumed (including a failed search).
     */
    virtual int erase(EnemyId id) = 0;

    /**
     * @brief Answers this structure's cheap native question (no policy
     * parameter).
     * @param out Set to the answer, if the structure is not empty.
     * @return Steps consumed.
     */
    virtual int query(EnemyId& out) const = 0;

    /**
     * @brief Number of tracked enemies. Not counted as steps.
     * @return Current size.
     */
    virtual std::size_t size() const = 0;

    /**
     * @brief Cumulative steps per category since the registry was built.
     * @return The registry's StepCounter.
     */
    virtual const StepCounter& stepBreakdown() const = 0;

    /**
     * @brief Diagnostic snapshot for the report; not counted as steps.
     * Only the trees (height) and the hash table (bucket lengths)
     * override it.
     * @return The current shape; defaults for other structures.
     */
    virtual RegistryShape shape() const {
      return RegistryShape{};
    }
};