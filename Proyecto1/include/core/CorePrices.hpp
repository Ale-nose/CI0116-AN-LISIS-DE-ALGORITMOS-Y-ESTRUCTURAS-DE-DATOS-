#pragma once 

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

constexpr int PRICE_LINKED_LIST = 0;
constexpr int PRICE_SORTED_LIST = 60;
constexpr int PRICE_DYNAMIC_ARRAY = 80;
constexpr int PRICE_SORTED_ARRAY = 150;
constexpr int PRICE_BST = 220;
constexpr int PRICE_AVL = 400;
constexpr int PRICE_MIN_HEAP = 260;
constexpr int PRICE_HASH_TABLE = 500;


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
