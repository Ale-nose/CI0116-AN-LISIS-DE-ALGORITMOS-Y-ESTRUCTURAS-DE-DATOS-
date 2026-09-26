// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "AvlRegistry.hpp"

AvlRegistry::AvlRegistry() : root(nullptr), count(0) {}

int AvlRegistry::heightOf(Node* node) {
  return node == nullptr ? 0 : node->height;
}

void AvlRegistry::updateHeight(Node* node) {
  int leftHeight = heightOf(node->left);
  int rightHeight = heightOf(node->right);
  node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int AvlRegistry::balanceFactor(Node* node) {
  return node == nullptr ? 0 : heightOf(node->left) - heightOf(node->right);
}

AvlRegistry::Node* AvlRegistry::rotateRight(Node* y) {
  Node* x = y->left;
  Node* t2 = x->right;
  x->right = y;
  y->left = t2;
  updateHeight(y);
  updateHeight(x);
  return x;
}

AvlRegistry::Node* AvlRegistry::rotateLeft(Node* x) {
  Node* y = x->right;
  Node* t2 = y->left;
  y->left = x;
  x->right = t2;
  updateHeight(x);
  updateHeight(y);
  return y;
}

AvlRegistry::Node* AvlRegistry::insertRec(Node* node, EnemyId id, Key k) {
  if (node == nullptr) {
    ++count;
    counter_.pointerHop();  // attach the new leaf
    return new Node{id, k, nullptr, nullptr, 1};
  }

  counter_.comparison();  // comparison at this level
  if (k == node->key) {
    node->id = id;
    return node;
  } else if (k < node->key) {
    node->left = insertRec(node->left, id, k);
  } else {
    node->right = insertRec(node->right, id, k);
  }

  updateHeight(node);
  int balance = balanceFactor(node);

  if (balance > 1 && k < node->left->key) {          // left-left
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && k > node->right->key) {         // right-right
    counter_.rotation();
    return rotateLeft(node);
  }
  if (balance > 1 && k > node->left->key) {           // left-right
    node->left = rotateLeft(node->left);
    counter_.rotation();
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && k < node->right->key) {         // right-left
    node->right = rotateRight(node->right);
    counter_.rotation();
    counter_.rotation();
    return rotateLeft(node);
  }

  return node;
}

AvlRegistry::Node* AvlRegistry::findMin(Node* node) const {
  while (node->left != nullptr) {
    counter_.pointerHop();
    node = node->left;
  }
  return node;
}

AvlRegistry::Node* AvlRegistry::eraseRec(Node* node, EnemyId id) {
  if (node == nullptr) {
    return nullptr;
  }

  counter_.comparison();
  if (id < node->key) {
    node->left = eraseRec(node->left, id);
  } else if (id > node->key) {
    node->right = eraseRec(node->right, id);
  } else {
    if (node->left == nullptr || node->right == nullptr) {
      Node* child = (node->left != nullptr) ? node->left : node->right;
      delete node;
      --count;
      counter_.pointerHop();  // splice the child in directly
      return child;
    }

    // Two children: pull up the in-order successor's data, then remove it.
    counter_.pointerHop();
    Node* successor = findMin(node->right);
    node->id = successor->id;
    node->key = successor->key;
    node->right = eraseRec(node->right, successor->id);
  }

  updateHeight(node);
  int balance = balanceFactor(node);

  if (balance > 1 && balanceFactor(node->left) >= 0) {
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance > 1 && balanceFactor(node->left) < 0) {
    node->left = rotateLeft(node->left);
    counter_.rotation();
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && balanceFactor(node->right) <= 0) {
    counter_.rotation();
    return rotateLeft(node);
  }
  if (balance < -1 && balanceFactor(node->right) > 0) {
    node->right = rotateRight(node->right);
    counter_.rotation();
    counter_.rotation();
    return rotateLeft(node);
  }

  return node;
}

void AvlRegistry::destroy(Node* node) {
  if (node == nullptr) return;
  destroy(node->left);
  destroy(node->right);
  delete node;
}

AvlRegistry::~AvlRegistry() {
  destroy(root);
}

int AvlRegistry::insert(EnemyId id, Key k) {
  const int before = counter_.total();
  root = insertRec(root, id, k);
  return counter_.total() - before;
}

int AvlRegistry::erase(EnemyId id) {
  const int before = counter_.total();
  root = eraseRec(root, id);
  return counter_.total() - before;
}

int AvlRegistry::query(EnemyId& out) const {
  const int before = counter_.total();
  if (root != nullptr) {
    // Native answer: true minimum, O(log n) thanks to balance.
    counter_.pointerHop();
    out = findMin(root)->id;
  }
  return counter_.total() - before;
}

size_t AvlRegistry::size() const {
  return count;
}