// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <array>

/**
 * @enum CoreType
 * @brief Identifies the available core registry structures for towers.
 */
enum class CoreType {
  LinkedList,
  SortedList,
  DynamicArray,
  SortedArray,
  Bst,
  Avl,
  MinHeap,
  HashTable
};

/**
 * @brief All 8 core types, in a fixed order. Useful for iterating over
 * every available structure (e.g. the upgrade dialog, section 5.3).
 */
inline const std::array<CoreType, 8>& allCoreTypes() {
  static constexpr std::array<CoreType, 8> kTypes = {
    CoreType::LinkedList,
    CoreType::SortedList,
    CoreType::DynamicArray,
    CoreType::SortedArray,
    CoreType::Bst,
    CoreType::Avl,
    CoreType::MinHeap,
    CoreType::HashTable,
  };
  return kTypes;
}

constexpr int PRICE_LINKED_LIST = 0;
constexpr int PRICE_SORTED_LIST = 60;
constexpr int PRICE_DYNAMIC_ARRAY = 80;
constexpr int PRICE_SORTED_ARRAY = 150;
constexpr int PRICE_BST = 220;
constexpr int PRICE_AVL = 400;
constexpr int PRICE_MIN_HEAP = 260;

/*
Raised from the original 500 to 800 (double the AVL price, the next most 
expensive structure). The hash table no longer needs a second, paired tower to 
target on its own, so its price should reflect that it now offers, alone, what 
used to take two towers to achieve.
*/
constexpr int PRICE_HASH_TABLE = 800;


/**
 * @brief Gets the credit price for a given core structure type.
 * @param type The core type.
 * @return The cost in credits for the specified core structure.
 */
inline int corePrice(CoreType type) {
  switch (type) {
    case CoreType::LinkedList:
      return PRICE_LINKED_LIST;
    case CoreType::SortedList:
      return PRICE_SORTED_LIST;
    case CoreType::DynamicArray:
      return PRICE_DYNAMIC_ARRAY;
    case CoreType::SortedArray:
      return PRICE_SORTED_ARRAY;
    case CoreType::Bst:
      return PRICE_BST;
    case CoreType::Avl:
      return PRICE_AVL;
    case CoreType::MinHeap:
      return PRICE_MIN_HEAP;
    case CoreType::HashTable:
      return PRICE_HASH_TABLE;
    default:
      return 0;
  }
}
