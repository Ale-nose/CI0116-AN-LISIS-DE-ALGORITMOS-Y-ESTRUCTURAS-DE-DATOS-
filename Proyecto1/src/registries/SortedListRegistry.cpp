// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
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
  const int before = counter_.total();
  Node* previous = nullptr;
  Node* current = head;

  while (current != nullptr) {
    counter_.comparison();
    if (!(current->key < k)) {
      break;  // found the insertion point
    }
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
  counter_.pointerHop();  // link the new node in
  ++count;
  return counter_.total() - before;
}

int SortedListRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  Node* previous = nullptr;
  Node* current = head;

  while (current != nullptr) {
    // One three-way comparison per node: past id's position (early
    // exit), a match, or keep walking.
    counter_.comparison();
    if (current->key > id) {
      break;
    }
    if (current->id == id) {
      if (previous == nullptr) {
        head = current->next;
      } else {
        previous->next = current->next;
      }
      counter_.pointerHop();  // splice the node out
      delete current;
      --count;
      break;
    }
    previous = current;
    counter_.pointerHop();
    current = current->next;
  }
  return counter_.total() - before;
}

int SortedListRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (head != nullptr) {
    counter_.pointerHop();  // native answer: head is the minimum, O(1)
    out = head->id;
  }
  return counter_.total() - before;
}

size_t SortedListRegistry::size() const {
  return count;
}