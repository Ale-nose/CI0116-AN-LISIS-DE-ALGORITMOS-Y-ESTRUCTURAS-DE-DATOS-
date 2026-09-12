// Common interface for all 8 targeting towers implementations
// Step logic:every operation (comparison, pointer movemement,
// shift, rotation) counts as exactly 1 step. Counts are kept per
// category so the log (Topic 6) can report them broken down
// by operation type, not just a single total.
#pragma once
#include <cstddef>

//
// TODO: Person B (Ashley) defines EnemyId and Key for real by generic
// parametrization by key type: id / distance / life). Placeholder
// typedefs below let this header compile on its own until then.
typedef int EnemyId;

typedef int Key;

// Per-operation step counter, shared by every concrete registry.
// total() is what gets returned by each interface method; the four
// category fields are what gets written to the combat log.
struct StepCounter {
    int comparisons = 0;
    int pointerHops  = 0;
    int shifts       = 0;
    int rotations    = 0;

    int total() const { 
      return comparisons + pointerHops + shifts + rotations;
    }
    
    void comparison(){ 
      ++comparisons; 
    }
    void pointerHop(){ 
      ++pointerHops; 
    }
    void shift(){ 
      ++shifts;
    }
    void rotation() {
      ++rotations;
    }
  };

// Pure interface: every method is a contract, not an implementation.
// Each of the 8 concrete registries inherits from this and fills in the actual logic.
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
};