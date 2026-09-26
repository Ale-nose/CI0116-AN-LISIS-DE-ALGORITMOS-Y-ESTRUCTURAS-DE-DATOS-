// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "DynamicArrayRegistry.hpp"

DynamicArrayRegistry::DynamicArrayRegistry()
    : data(new Entry[4]), capacity(4), count(0) {}

DynamicArrayRegistry::~DynamicArrayRegistry() {
  delete[] data;
}

int DynamicArrayRegistry::insert(EnemyId id, Key k) {
  const int before = counter_.total();
  if (count == capacity) {
    int newCapacity = capacity * 2;
    Entry* newData = new Entry[newCapacity];
    for (int i = 0; i < count; ++i) {
      newData[i] = data[i];
      counter_.shift();  // copying during growth
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
  }
  data[count] = Entry{id, k};
  counter_.shift();  // the append write
  ++count;
  return counter_.total() - before;
}

int DynamicArrayRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  int index = -1;
  for (int i = 0; i < count; ++i) {
    counter_.comparison();
    if (data[i].id == id) {
      index = i;
      break;
    }
  }
  if (index != -1) {
    data[index] = data[count - 1];  // swap with the last element
    counter_.shift();
    --count;
  }
  return counter_.total() - before;
}

int DynamicArrayRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (count > 0) {
    counter_.pointerHop();  // native answer: peek the last slot, O(1)
    out = data[count - 1].id;
  }
  return counter_.total() - before;
}

size_t DynamicArrayRegistry::size() const {
  return static_cast<std::size_t>(count);
}