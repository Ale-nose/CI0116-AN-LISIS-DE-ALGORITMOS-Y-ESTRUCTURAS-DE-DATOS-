// Copyright 2026 Kevin Velásquez García
#include "SortedListRegistry.hpp"

SortedListRegistry::SortedListRegistry() : head(nullptr), count(0) {}

SortedListRegistry::~SortedListRegistry() {
  Node* current = head;
  while (current != nullptr) {
    Node* next = current->next;
    delete current;
    current = next;
  }
}

int SortedListRegistry::insert(EnemyId id, Key k) {
  int steps = 0;
  Node* previous = nullptr;
  Node* current = head;

  while (current != nullptr && current->key < k) {
    ++steps;
    counter_.comparison();
    previous = current;
    counter_.pointerHop();
    current = current->next;
  }

  Node* node = new Node{id, k, current};
  if (previous == nullptr) {
    head = node;
  } else {
    previous->next = node;
  }
  ++steps;
  ++count;
  return steps;
}

int SortedListRegistry::erase(EnemyId id) {
  int steps = 0;
  Node* previous = nullptr;
  Node* current = head;

  // Early exit: stop as soon as we pass where id's key would be.
  while (current != nullptr && !(current->key > id)) {
    ++steps;
    counter_.comparison();
    if (current->id == id) {
      if (previous == nullptr) {
        head = current->next;
      } else {
        previous->next = current->next;
      }
      ++steps;
      delete current;
      --count;
      return steps;
    }
    previous = current;
    counter_.pointerHop();
    current = current->next;
  }
  return steps;
}

int SortedListRegistry::query(EnemyId& out) const {
  if (head == nullptr) {
    return 0;
  }
  out = head->id;
  return 1;  // native answer: head is the minimum, O(1)
}

size_t SortedListRegistry::size() const {
  return count;
}
