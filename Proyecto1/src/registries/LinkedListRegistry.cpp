// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
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
  const int before = counter_.total();
  head = new Node{id, k, head};
  counter_.pointerHop();  // attach the new node at the front
  ++count;
  return counter_.total() - before;
}

int LinkedListRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  Node* previous = nullptr;
  Node* current = head;

  while (current != nullptr) {
    counter_.comparison();
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

int LinkedListRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (head != nullptr) {
    counter_.pointerHop();  // native answer: peek the head, O(1)
    out = head->id;
  }
  return counter_.total() - before;
}

size_t LinkedListRegistry::size() const {
  return count;
}