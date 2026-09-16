// Copyright 2026 Kevin Velásquez García
#include "DynamicArrayRegistry.hpp"

DynamicArrayRegistry::DynamicArrayRegistry()
    : data(new Entry[4]), capacity(4), count(0) {}

DynamicArrayRegistry::~DynamicArrayRegistry() {
  delete[] data;
}

int DynamicArrayRegistry::insert(EnemyId id, Key k) {
  int steps = 0;
  if (count == capacity) {
    int newCapacity = capacity * 2;
    Entry* newData = new Entry[newCapacity];
    for (int i = 0; i < count; ++i) {
      newData[i] = data[i];
      ++steps;  // one step per element copied while growing
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
  }
  data[count] = Entry{id, k};
  ++count;
  ++steps;  // the final write
  return steps;
}

int DynamicArrayRegistry::erase(EnemyId id) {
  int steps = 0;
  int index = -1;
  for (int i = 0; i < count; ++i) {
    ++steps;
    if (data[i].id == id) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    return steps;  // not found: walked the whole array
  }
  data[index] = data[count - 1];  // swap with the last element
  ++steps;
  --count;
  return steps;
}

int DynamicArrayRegistry::query(EnemyId& out) const {
  if (count == 0) {
    return 0;
  }
  out = data[count - 1].id;
  return 1;  // native answer: peek the last slot, O(1)
}

size_t DynamicArrayRegistry::size() const {
  return static_cast<std::size_t>(count);
}
