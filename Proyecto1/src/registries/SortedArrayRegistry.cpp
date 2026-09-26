// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "SortedArrayRegistry.hpp"

SortedArrayRegistry::SortedArrayRegistry()
    : data(new Entry[4]), capacity(4), count(0) {}

SortedArrayRegistry::~SortedArrayRegistry() {
  delete[] data;
}

void SortedArrayRegistry::grow() {
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

SortedArrayRegistry::SearchResult SortedArrayRegistry::binarySearch(
  Key key) const {
  int low = 0;
  int high = count - 1;

  while (low <= high) {
    counter_.comparison();  // one three-way comparison per probe

    int mid = low + (high - low) / 2;
    if (data[mid].key == key) {
      return {mid, true};
    } else if (data[mid].key < key) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return {low, false};
}

int SortedArrayRegistry::insert(EnemyId id, Key k) {
  const int before = counter_.total();
  SearchResult result = binarySearch(k);

  if (count == capacity) {
    grow();
  }

  for (int i = count; i > result.index; --i) {
    data[i] = data[i - 1];
    counter_.shift();  // one step per shifted element
  }

  data[result.index] = Entry{id, k};
  counter_.shift();  // the final write
  ++count;
  return counter_.total() - before;
}

int SortedArrayRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  SearchResult result = binarySearch(id);
  if (result.found) {
    for (int i = result.index; i < count - 1; ++i) {
      data[i] = data[i + 1];
      counter_.shift();  // one step per shifted element
    }
    --count;
  }
  return counter_.total() - before;
}

int SortedArrayRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (count > 0) {
    counter_.pointerHop();  // native answer: index 0 is the minimum, O(1)
    out = data[0].id;
  }
  return counter_.total() - before;
}

size_t SortedArrayRegistry::size() const {
  return static_cast<std::size_t>(count);
}