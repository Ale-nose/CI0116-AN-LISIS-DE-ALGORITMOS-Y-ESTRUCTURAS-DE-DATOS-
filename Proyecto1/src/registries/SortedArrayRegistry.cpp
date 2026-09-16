// Copyright 2026 Kevin Velásquez García
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
  }
  delete[] data;
  data = newData;
  capacity = newCapacity;
}

SortedArrayRegistry::SearchResult SortedArrayRegistry::binarySearch(Key key) const {
  int steps = 0;
  int low = 0;
  int high = count - 1;

  while (low <= high) {
    ++steps;
    int mid = low + (high - low) / 2;
    if (data[mid].key == key) {
      return {mid, true, steps};
    } else if (data[mid].key < key) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return {low, false, steps};
}

int SortedArrayRegistry::insert(EnemyId id, Key k) {
  SearchResult result = binarySearch(k);
  int steps = result.steps;

  if (count == capacity) {
    grow();
  }

  for (int i = count; i > result.index; --i) {
    data[i] = data[i - 1];
    ++steps;  // one step per shifted element
  }

  data[result.index] = Entry{id, k};
  ++count;
  ++steps;  // the final write
  return steps;
}

int SortedArrayRegistry::erase(EnemyId id) {
  SearchResult result = binarySearch(id);
  if (!result.found) {
    return result.steps;  // just the search, nothing else to do
  }

  int steps = result.steps;
  for (int i = result.index; i < count - 1; ++i) {
    data[i] = data[i + 1];
    ++steps;  // one step per shifted element
  }
  --count;
  return steps;
}

int SortedArrayRegistry::query(EnemyId& out) const {
  if (count == 0) {
    return 0;
  }
  out = data[0].id;
  return 1;  // native answer: index 0 is the minimum, O(1)
}

size_t SortedArrayRegistry::size() const {
  return static_cast<std::size_t>(count);
}
