#include "Tower.hpp"

#include <chrono>

Tower::Tower(std::unique_ptr<ITargetRegistry> registry)
    : registry_(std::move(registry))
{
}

void Tower::installRegistry(std::unique_ptr<ITargetRegistry> registry)
{
    registry_ = std::move(registry);
    // Drop pending maintenance: it referred to the previous core.
    std::queue<MaintenanceOp> empty;
    std::swap(pending_, empty);
}

void Tower::enqueueInsert(EnemyId id, Key key)
{
    pending_.push(MaintenanceOp{MaintOpType::Insert, id, key});
}

void Tower::enqueueErase(EnemyId id)
{
    pending_.push(MaintenanceOp{MaintOpType::Erase, id, std::nullopt});
}

bool Tower::tick(EnemyId& firedTarget, int* stepsUsedOut)
{
    if (stepsUsedOut)
    {
        *stepsUsedOut = 0;
    }

    if (busy_ticks_ > 0)
    {
        --busy_ticks_;
        return false;  // still paying for a previous operation
    }

    int steps = 0;
    bool fired = false;

    auto start = std::chrono::high_resolution_clock::now();

    if (!pending_.empty())
    {
        MaintenanceOp op = pending_.front();
        pending_.pop();
        steps = (op.type == MaintOpType::Insert)
            ? registry_->insert(op.id, *op.key)
            : registry_->erase(op.id);
    }
    else if (registry_->size() > 0)
    {
        // An empty registry has nothing to shoot at: querying it still
        // costs 0 steps and leaves firedTarget untouched, so don't count
        // it as a shot.
        steps = registry_->query(firedTarget);
        fired = true;
    }

    auto end = std::chrono::high_resolution_clock::now();
    total_microseconds_ += std::chrono::duration_cast
        <std::chrono::microseconds>(end - start).count();
        
    // ceil(steps / STEPS_PER_TICK) without <cmath>.
    busy_ticks_ = (steps + STEPS_PER_TICK - 1) / STEPS_PER_TICK;

    if (stepsUsedOut)
    {
        *stepsUsedOut = steps;
    }

    return fired;
}
