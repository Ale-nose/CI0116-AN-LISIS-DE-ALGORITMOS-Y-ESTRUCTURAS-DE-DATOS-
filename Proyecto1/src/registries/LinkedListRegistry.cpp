// Copyright 2026 Kevin Velásquez García
#include "LinkedListRegistry.hpp"

LinkedListRegistry::LinkedListRegistry() : head(nullptr), count(0) {}

LinkedListRegistry::~LinkedListRegistry() {
  Node* current = head;
  while (current != nullptr) {
    Node* next = current->next;
    delete current;
    current = next;
  }
}

int LinkedListRegistry::insert(EnemyId id, Key k) {
  head = new Node{id, k, head};
  ++count;
  return 1;
}

int LinkedListRegistry::erase(EnemyId id) {
  int steps = 0;
  Node* previous = nullptr;
  Node* current = head;

  while (current != nullptr) {
    ++steps;
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
    current = current->next;
  }
  return steps;
}

int LinkedListRegistry::query(EnemyId& out) const {
  if (head == nullptr) {
    return 0;
  }
  out = head->id;
  return 1;  // native answer: peek the head, O(1)
}

size_t LinkedListRegistry::size() const {
  return count;
}
