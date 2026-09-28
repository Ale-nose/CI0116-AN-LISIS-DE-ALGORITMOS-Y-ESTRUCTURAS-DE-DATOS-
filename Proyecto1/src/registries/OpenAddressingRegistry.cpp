// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "OpenAddressingRegistry.hpp"

#include <functional>

namespace {
constexpr int kInitialCapacity = 11;
constexpr double kMaxLoadFactor = 0.5;
}  // namespace

OpenAddressingRegistry::OpenAddressingRegistry(ProbingMode mode)
  : table_(kInitialCapacity)
  , mode_(mode) {
}

int OpenAddressingRegistry::primaryHash(Key key) const {
  return static_cast<int>(std::hash<Key>{}(key) % table_.size());
}

int OpenAddressingRegistry::secondaryHash(Key key) const {
  return 1 + static_cast<int>(std::hash<Key>{}(key) % (table_.size() - 1));
}

bool OpenAddressingRegistry::loadFactorTooHigh() const {
  return static_cast<double>(count_ + 1) / table_.size() > kMaxLoadFactor;
}

void OpenAddressingRegistry::rehash() {
  std::vector<Slot> old = std::move(table_);
  table_.assign(old.size() * 2 + 1, Slot{});
  count_ = 0;

  for (const Slot& slot : old) {
    if (slot.occupied && !slot.deleted) {
      insert(slot.id, slot.id);
    }
  }
}

int OpenAddressingRegistry::findSlot(Key key, bool forInsert) const {
  int step = (mode_ == ProbingMode::Double) ? secondaryHash(key) : 1;
  int index = primaryHash(key);
  int firstTombstone = -1;

  for (std::size_t probes = 0; probes < table_.size(); ++probes) {
    ++totalProbes_;
    counter_.comparison();
    const Slot& slot = table_[index];

    if (!slot.occupied) {
      return forInsert && firstTombstone != -1 ? firstTombstone : index;
    }

    if (slot.deleted) {
      if (firstTombstone == -1) {
        firstTombstone = index;
      }
    } else if (slot.id == key) {
      return index;
    }

    counter_.pointerHop();
    index = static_cast<int>((index + step) % table_.size());
  }
  return forInsert && firstTombstone != -1 ? firstTombstone : -1;
}

int OpenAddressingRegistry::insert(EnemyId id, Key k) {
  const int before = counter_.total();
  ++totalOps_;
  if (loadFactorTooHigh()) {
    rehash();
  }

  int index = findSlot(k, true);
  table_[index] = Slot{id, true, false};
  ++count_;
  return counter_.total() - before;
}

int OpenAddressingRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  ++totalOps_;
  int index = findSlot(id, false);

  if (index != -1) {
    table_[index].deleted = true;
    --count_;
  }
  return counter_.total() - before;
}

int OpenAddressingRegistry::query(EnemyId& out) const {
  const int before = counter_.total();

  for (const Slot& slot : table_) {
    counter_.comparison();
    if (slot.occupied && !slot.deleted) {
      out = slot.id;
      break;
    }
  }

  return counter_.total() - before;
}

size_t OpenAddressingRegistry::size() const {
  return static_cast<std::size_t>(count_);
}
