// Copyright 2026 Kevin Velásquez García
#include "HashTableRegistry.hpp"

#include <functional>
#include <utility>

HashTableRegistry::HashTableRegistry() 
  : buckets(kInitialBucketCount), count(0) {}

int HashTableRegistry::bucketIndex(Key key) const {
  return static_cast<int>(std::hash<Key>{}(key) % buckets.size());
}

bool HashTableRegistry::loadFactorTooHigh() const {
  return static_cast<double>(count + 1) / buckets.size() > kMaxLoadFactor;
}

int HashTableRegistry::rehash() {
  int steps = 0;
  std::vector<std::vector<Entry>> oldBuckets = std::move(buckets);
  buckets.assign(oldBuckets.size() * 2, {});

  for (std::vector<Entry>& chain : oldBuckets) {
    for (Entry& entry : chain) {
      int index = bucketIndex(entry.key);
      buckets[index].push_back(entry);
      ++steps;
      counter_.shift();  // moving an element during rehash
    }
  }
  return steps;
}

int HashTableRegistry::insert(EnemyId id, Key k) {
  int steps = 0;
  if (loadFactorTooHigh()) {
    steps += rehash();
  }

  int index = bucketIndex(k);
  ++steps;  // computing the hash

  std::vector<Entry>& chain = buckets[index];
  for (Entry& entry : chain) {
    ++steps;
    counter_.comparison();
    if (entry.id == id) {
      entry.key = k;  // key already exists, just update
      return steps;
    }
  }
  chain.push_back(Entry{id, k});
  ++steps;
  ++count;
  return steps;
}

int HashTableRegistry::erase(EnemyId id) {
  int steps = 0;
  // id doubles as the key (spec: targeting is by id only), so the
  // bucket it hashed into is found the same way it was inserted.
  int index = bucketIndex(id);
  ++steps;

  std::vector<Entry>& chain = buckets[index];
  for (auto it = chain.begin(); it != chain.end(); ++it) {
    ++steps;
    counter_.comparison();
    if (it->id == id) {
      chain.erase(it);
      ++steps;
      --count;
      return steps;
    }
  }
  return steps;  // not found: walked the whole bucket
}

int HashTableRegistry::query(EnemyId& out) const {
  int steps = 0;
  for (const std::vector<Entry>& chain : buckets) {
    ++steps;  // checking whether this bucket is empty
    counter_.comparison();  // checking if bucket empty
    if (!chain.empty()) {
      out = chain.front().id;
      return steps;
    }
  }
  return steps;  // registry is empty
}

size_t HashTableRegistry::size() const {
  return static_cast<std::size_t>(count);
}
