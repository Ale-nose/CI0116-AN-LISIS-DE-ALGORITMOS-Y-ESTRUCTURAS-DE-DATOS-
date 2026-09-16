#pragma once
#include <memory>
#include <optional>
#include <queue>
#include "TargetRegistry.hpp"
#include "Ticks.hpp"  // for STEPS_PER_TICK

// Kind of pending maintenance operation queued for a tower.
enum class MaintOpType
{
    Insert,
    Erase
};

struct MaintenanceOp
{
    MaintOpType type;
    EnemyId id;
    std::optional<Key> key;  // set only when type == Insert
};

// Within each tick, a FREE tower takes exactly ONE pending maintenance
// operation if any exist; otherwise it fires a query. It computes the
// step cost and becomes blocked for ceil(cost / STEPS_PER_TICK) ticks.
// A busy tower does nothing and just counts down. There is no per-tick
// budget that resets.
class Tower
{
public:
    explicit Tower(std::unique_ptr<ITargetRegistry> registry);

    // Swap the structure installed in this slot. Any operations still
    // queued for the old core are dropped, since they no longer apply
    // to the new one.
    void installRegistry(std::unique_ptr<ITargetRegistry> registry);

    // Called by Simulation when an enemy enters range, to schedule an
    // insert for this tower's core. key is the value this core indexes
    // by (identifier, distance, or life — same value as id when the
    // core is keyed by identifier).
    void enqueueInsert(EnemyId id, Key key);

    // Called by Simulation when an enemy leaves range or dies, to
    // schedule an erase for this tower's core.
    void enqueueErase(EnemyId id);

    // Runs one tick for this tower.
    // - If still blocked from a previous operation, decrements the block
    //   counter and does nothing else this tick.
    // - If free, takes one pending maintenance op if any, otherwise
    //   fires via query(). Computes the step cost and blocks for
    //   ceil(cost / STEPS_PER_TICK) ticks.
    // Returns true and sets firedTarget only when a shot was taken THIS
    // tick (a maintenance tick or a blocked tick never fires).
    // stepsUsedOut, if non-null, receives the steps spent this tick (0 if
    // the tower was blocked and did nothing), for the combat log.
    bool tick(EnemyId& firedTarget, int* stepsUsedOut = nullptr);

    bool isBusy() const
    {
        return busy_ticks_ > 0;
    }

    std::size_t registrySize() const
    {
        return registry_->size();
    }

    bool hasPendingMaintenance() const
    {
        return !pending_.empty();
    }

private:
    std::unique_ptr<ITargetRegistry> registry_;
    std::queue<MaintenanceOp> pending_;
    int busy_ticks_ = 0;
};