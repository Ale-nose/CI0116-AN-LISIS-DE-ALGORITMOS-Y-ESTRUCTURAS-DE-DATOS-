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

AvlRegistry::Node* AvlRegistry::insertRec(
  Node* node, EnemyId id, Key k, int& steps) {
  if (node == nullptr) {
    ++count;
    ++steps;
    counter_.pointerHop();
    return new Node{id, k, nullptr, nullptr, 1};
  }

  ++steps;  // comparison at this level
  counter_.comparison();
  if (k == node->key) {
    node->id = id;
    return node;
  } else if (k < node->key) {
    node->left = insertRec(node->left, id, k, steps);
  } else {
    node->right = insertRec(node->right, id, k, steps);
  }

  updateHeight(node);
  int balance = balanceFactor(node);

  if (balance > 1 && k < node->left->key) {          // left-left
    ++steps;
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && k > node->right->key) {         // right-right
    ++steps;
    counter_.rotation();
    return rotateLeft(node);
  }
  if (balance > 1 && k > node->left->key) {           // left-right
    node->left = rotateLeft(node->left);
    ++steps;
    counter_.rotation();
    ++steps;
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && k < node->right->key) {         // right-left
    node->right = rotateRight(node->right);
    ++steps;
    counter_.rotation();
    ++steps;
    counter_.rotation();
    return rotateLeft(node);
  }

  return node;
}

AvlRegistry::Node* AvlRegistry::findMin(Node* node, int& steps) const {
  while (node->left != nullptr) {
    ++steps;
    counter_.pointerHop();
    node = node->left;
  }
  return node;
}

AvlRegistry::Node* AvlRegistry::eraseRec(
  Node* node, EnemyId id, int& steps, bool& found) {
  if (node == nullptr) {
    return nullptr;
  }

  ++steps;
  counter_.comparison();
  if (id < node->key) {
    node->left = eraseRec(node->left, id, steps, found);
  } else if (id > node->key) {
    node->right = eraseRec(node->right, id, steps, found);
  } else {
    found = true;
    if (node->left == nullptr || node->right == nullptr) {
      Node* child = (node->left != nullptr) ? node->left : node->right;
      delete node;
      --count;
      return child;  // 0 or 1 child: splice it in directly
    }

    // Two children: pull up the in-order successor's data, then remove it.
    Node* successor = findMin(node->right, steps);
    node->id = successor->id;
    node->key = successor->key;
    bool dummy = false;
    node->right = eraseRec(node->right, successor->id, steps, dummy);
  }

  if (node == nullptr) {
    return nullptr;
  }

  updateHeight(node);
  int balance = balanceFactor(node);

  if (balance > 1 && balanceFactor(node->left) >= 0) {
    ++steps;
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance > 1 && balanceFactor(node->left) < 0) {
    node->left = rotateLeft(node->left);
    ++steps;
    counter_.rotation();
    ++steps;
    counter_.rotation();
    return rotateRight(node);
  }
  if (balance < -1 && balanceFactor(node->right) <= 0) {
    ++steps;
    counter_.rotation();
    return rotateLeft(node);
  }
  if (balance < -1 && balanceFactor(node->right) > 0) {
    node->right = rotateRight(node->right);
    ++steps;
    counter_.rotation();
    ++steps;
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
  int steps = 0;
  root = insertRec(root, id, k, steps);
  return steps;
}

int AvlRegistry::erase(EnemyId id) {
  int steps = 0;
  bool found = false;
  root = eraseRec(root, id, steps, found);
  return steps;
}

int AvlRegistry::query(EnemyId& out) const {
  if (root == nullptr) {
    return 0;
  }
  int steps = 1;
  Node* minNode = findMin(root, steps);
  out = minNode->id;
  return steps;  // native answer: true minimum, O(log n) thanks to balance
}

size_t AvlRegistry::size() const {
  return count;
}
