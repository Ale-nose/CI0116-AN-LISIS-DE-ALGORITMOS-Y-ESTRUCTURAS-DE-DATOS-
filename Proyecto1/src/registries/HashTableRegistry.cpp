// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
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

void HashTableRegistry::rehash() {
  std::vector<std::vector<Entry>> oldBuckets = std::move(buckets);
  buckets.assign(oldBuckets.size() * 2, {});

  for (std::vector<Entry>& chain : oldBuckets) {
    for (Entry& entry : chain) {
      int index = bucketIndex(entry.key);
      buckets[index].push_back(entry);
      counter_.shift();  // moving an element during rehash
    }
  }
}

int HashTableRegistry::insert(EnemyId id, Key k) {
  const int before = counter_.total();
  if (loadFactorTooHigh()) {
    rehash();
  }

  int index = bucketIndex(k);
  counter_.pointerHop();  // jump from the hash to its bucket

  std::vector<Entry>& chain = buckets[index];
  for (Entry& entry : chain) {
    counter_.comparison();
    if (entry.id == id) {
      entry.key = k;  // key already exists, just update
      return counter_.total() - before;
    }
  }
  chain.push_back(Entry{id, k});
  counter_.shift();  // the append write into the chain
  ++count;
  return counter_.total() - before;
}

int HashTableRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  // id doubles as the key (spec: targeting is by id only), so the
  // bucket it hashed into is found the same way it was inserted.
  int index = bucketIndex(id);
  counter_.pointerHop();  // jump from the hash to its bucket

  std::vector<Entry>& chain = buckets[index];
  for (auto it = chain.begin(); it != chain.end(); ++it) {
    counter_.comparison();
    if (it->id == id) {
      // vector::erase shifts every later element one slot left.
      for (auto later = it + 1; later != chain.end(); ++later) {
        counter_.shift();
      }
      chain.erase(it);
      --count;
      break;
    }
  }
  return counter_.total() - before;
}

int HashTableRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  for (const std::vector<Entry>& chain : buckets) {
    counter_.comparison();  // checking whether this bucket is empty
    if (!chain.empty()) {
      out = chain.front().id;
      break;
    }
  }
  return counter_.total() - before;
}

size_t HashTableRegistry::size() const {
  return static_cast<std::size_t>(count);
}