// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
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
  const int before = counter_.total();
  if (root == nullptr) {
    root = new Node{id, k, nullptr, nullptr};
    counter_.pointerHop();  // attach as root
    ++count;
    return counter_.total() - before;
  }

  Node* current = root;
  while (true) {
    counter_.comparison();
    if (k == current->key) {
      current->id = id;  // key already present, just update
      break;
    } else if (k < current->key) {
      counter_.pointerHop();  // go to (or attach at) the left child
      if (current->left == nullptr) {
        current->left = new Node{id, k, nullptr, nullptr};
        ++count;
        break;
      }
      current = current->left;
    } else {
      counter_.pointerHop();  // go to (or attach at) the right child
      if (current->right == nullptr) {
        current->right = new Node{id, k, nullptr, nullptr};
        ++count;
        break;
      }
      current = current->right;
    }
  }
  return counter_.total() - before;
}

int BstRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  Node* parent = nullptr;
  Node* current = root;

  while (current != nullptr) {
    counter_.comparison();
    if (current->id == id) {
      break;
    }
    parent = current;
    counter_.pointerHop();
    current = (id < current->key) ? current->left : current->right;
  }

  if (current == nullptr) {
    return counter_.total() - before;  // not found: full path already walked
  }

  if (current->left != nullptr && current->right != nullptr) {
    // Two children: replace with the in-order successor (min of right subtree).
    Node* successorParent = current;
    counter_.pointerHop();
    Node* successor = current->right;
    while (successor->left != nullptr) {
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

  counter_.pointerHop();  // the unlink itself
  --count;
  return counter_.total() - before;
}

int BstRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (root != nullptr) {
    // Native answer: peek the root, O(1), not necessarily the min.
    counter_.pointerHop();
    out = root->id;
  }
  return counter_.total() - before;
}

size_t BstRegistry::size() const {
  return count;
}