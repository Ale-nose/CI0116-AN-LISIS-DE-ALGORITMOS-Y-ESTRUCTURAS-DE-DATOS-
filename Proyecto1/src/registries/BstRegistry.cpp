// Copyright 2026 Kevin Velásquez García
#include "BstRegistry.hpp"

BstRegistry::BstRegistry() : root(nullptr), count(0) {}

void BstRegistry::destroy(Node* node) {
  if (node == nullptr) return;
  destroy(node->left);
  destroy(node->right);
  delete node;
}

BstRegistry::~BstRegistry() {
  destroy(root);
}

int BstRegistry::insert(EnemyId id, Key k) {
  if (root == nullptr) {
    root = new Node{id, k, nullptr, nullptr};
    ++count;
    return 1;
  }

  int steps = 0;
  Node* current = root;
  while (true) {
    ++steps;
    counter_.comparison();
    if (k == current->key) {
      current->id = id;  // key already present, just update
      return steps;
    } else if (k < current->key) {
      if (current->left == nullptr) {
        current->left = new Node{id, k, nullptr, nullptr};
        ++steps;
        ++count;
        return steps;
      }
      counter_.pointerHop();
      current = current->left;
    } else {
      if (current->right == nullptr) {
        current->right = new Node{id, k, nullptr, nullptr};
        ++steps;
        ++count;
        return steps;
      }
      counter_.pointerHop();
      current = current->right;
    }
  }
}

int BstRegistry::erase(EnemyId id) {
  int steps = 0;
  Node* parent = nullptr;
  Node* current = root;

  while (current != nullptr && current->id != id) {
    ++steps;
    counter_.comparison();

    parent = current;
    counter_.pointerHop();
    current = (id < current->key) ? current->left : current->right;
  }

  if (current == nullptr) {
    return steps;  // not found: full path already walked
  }
  ++steps;  // the comparison that confirmed the match
  counter_.comparison();

  if (current->left != nullptr && current->right != nullptr) {
    // Two children: replace with the in-order successor (min of right subtree).
    Node* successorParent = current;
    Node* successor = current->right;
    while (successor->left != nullptr) {
      ++steps;
      counter_.pointerHop();
      successorParent = successor;
      successor = successor->left;
    }

    current->id = successor->id;
    current->key = successor->key;

    Node* child = successor->right;  // successor has at most a right child
    if (successorParent->left == successor) {
      successorParent->left = child;
    } else {
      successorParent->right = child;
    }
    delete successor;
  } else {
    // 0 or 1 child: splice it in directly.
    Node* child = (current->left != nullptr) ? current->left : current->right;
    if (parent == nullptr) {
      root = child;
    } else if (parent->left == current) {
      parent->left = child;
    } else {
      parent->right = child;
    }
    delete current;
  }

  ++steps;  // the unlink itself
  --count;
  return steps;
}

int BstRegistry::query(EnemyId& out) const {
  if (root == nullptr) {
    return 0;
  }
  out = root->id;
  return 1;  // native answer: peek the root, O(1), not necessarily the min
}

size_t BstRegistry::size() const {
  return count;
}
