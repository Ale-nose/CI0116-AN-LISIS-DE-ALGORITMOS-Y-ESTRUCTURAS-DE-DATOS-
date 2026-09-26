// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MinHeapRegistry.hpp"

#include <utility>

MinHeapRegistry::MinHeapRegistry() : count(0) {}

int MinHeapRegistry::siftUp(int index) {
  while (index > 0) {
    int parent = (index - 1) / 2;
    counter_.comparison();
    if (data[parent].key <= data[index].key) break;

    std::swap(data[parent], data[index]);
    counter_.shift();
    index = parent;
  }
  return index;
}

int MinHeapRegistry::siftDown(int index) {
  while (true) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < count) {
      counter_.comparison();
      if (data[left].key < data[smallest].key) smallest = left;
    }
    if (right < count) {
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
  const int before = counter_.total();
  data.push_back(Entry{id, k});
  counter_.shift();  // the append write
  ++count;
  siftUp(count - 1);
  return counter_.total() - before;
}

int MinHeapRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  int index = -1;
  for (int i = 0; i < count; ++i) {
    // Linear search: a min-heap has no order besides "root is min".
    counter_.comparison();
    if (data[i].id == id) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    return counter_.total() - before;
  }

  data[index] = data[count - 1];
  data.pop_back();
  --count;
  counter_.shift();  // move the last element into the hole

  if (index < count) {
    int afterUp = siftUp(index);
    if (afterUp == index) {
      siftDown(index);
    }
  }
  return counter_.total() - before;
}

int MinHeapRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (count > 0) {
    counter_.pointerHop();  // native answer: the root is the true minimum, O(1)
    out = data[0].id;
  }
  return counter_.total() - before;
}

size_t MinHeapRegistry::size() const {
  return static_cast<std::size_t>(count);
}