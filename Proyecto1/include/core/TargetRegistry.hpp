// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
// Common interface for all 8 targeting towers implementations
// Step logic:every operation (comparison, pointer movemement,
// shift, rotation) counts as exactly 1 step. Counts are kept per
// category so the log (Topic 6) can report them broken down
// by operation type, not just a single total.
#pragma once

#include <cstddef>
#include "KeyType.hpp"

// Per-operation step counter, shared by every concrete registry.
// Every step an operation performs is recorded here exactly once, and
// each interface method returns counter_.total() after the operation
// minus counter_.total() before it — so the cost that blocks a tower,
// the log's steps_total and the sum of the four categories are always
// the same number. Category rules, applied identically by all 8:
//  - comparison(): examining one element/key/bucket (a three-way
//    compare of the same pair counts once).
//  - pointerHop(): following or rewriting a link to reach or attach an
//    element: walking to the next node/child, splicing or attaching a
//    node, jumping from a hash to its bucket, peeking a known slot.
//  - shift(): writing an element into an array position: shifting,
//    swapping, appending, or copying during grow/rehash.
//  - rotation(): one tree rotation.
struct StepCounter {
  int comparisons = 0;
  int pointerHops  = 0;
  int shifts       = 0;
  int rotations    = 0;

  int total() const {
    return comparisons + pointerHops + shifts + rotations;
  }

  void comparison() {
    ++comparisons;
  }
  void pointerHop() {
    ++pointerHops;
  }
  void shift() {
    ++shifts;
  }
  void rotation() {
    ++rotations;
  }
};

// Pure interface: every method is a contract, not an implementation.
// Each of the 8 concrete registries inherits
// from this and fills in the actual logic.
class ITargetRegistry {
 public:
    virtual ~ITargetRegistry() = default;

    // Inserts id with key k. Returns steps consumed.
    virtual int insert(EnemyId id, Key k) = 0;

    // Removes id. Returns steps consumed.
    virtual int erase(EnemyId id) = 0;

    // Answers this structure's cheap native question (no policy
    // parameter). out is set to the result if one exists.
    // Returns steps consumed.
    virtual int query(EnemyId& out) const = 0;

    // Current number of tracked enemies. Not counted as steps.
    virtual std::size_t size() const = 0;

    virtual const StepCounter& stepBreakdown() const = 0;
};