// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MinHeapRegistry.hpp"

#include <utility>

MinHeapRegistry::MinHeapRegistry() : count(0) {}

int MinHeapRegistry::siftUp(int index, int& steps) {
  while (index > 0) {
    int parent = (index - 1) / 2;
    ++steps;
    counter_.comparison();
    if (data[parent].key <= data[index].key) break;

    std::swap(data[parent], data[index]);
    counter_.shift();
    index = parent;
  }
  return index;
}

int MinHeapRegistry::siftDown(int index, int& steps) {
  while (true) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < count) {
      ++steps;
      counter_.comparison();
      if (data[left].key < data[smallest].key) smallest = left;
    }
    if (right < count) {
      ++steps;
      counter_.comparison();
      if (data[right].key < data[smallest].key) smallest = right;
    }
    if (smallest == index) break;
    std::swap(data[index], data[smallest]);
    counter_.shift();
    index = smallest;
  }
  return index;
}

int MinHeapRegistry::insert(EnemyId id, Key k) {
  data.push_back(Entry{id, k});
  ++count;
  int steps = 1;  // the append itself
  siftUp(count - 1, steps);
  return steps;
}

int MinHeapRegistry::erase(EnemyId id) {
  int steps = 0;
  int index = -1;
  for (int i = 0; i < count; ++i) {
    ++steps;  // linear search: a min-heap has no order besides "root is min"
    counter_.comparison();
    if (data[i].id == id) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    return steps;
  }

  data[index] = data[count - 1];
  data.pop_back();
  --count;
  ++steps;  // move the last element into the hole
  counter_.shift();

  if (index < count) {
    int afterUp = siftUp(index, steps);
    if (afterUp == index) {
      siftDown(index, steps);
    }
  }
  return steps;
}

int MinHeapRegistry::query(EnemyId& out) const {
  if (count == 0) {
    return 0;
  }
  out = data[0].id;
  return 1;  // native answer: the root is the true minimum, O(1)
}

size_t MinHeapRegistry::size() const {
  return static_cast<std::size_t>(count);
}
