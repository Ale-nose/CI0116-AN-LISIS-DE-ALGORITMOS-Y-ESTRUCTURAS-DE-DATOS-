#include "CoreFactory.hpp"

#include "AvlRegistry.hpp"
#include "BstRegistry.hpp"
#include "DynamicArrayRegistry.hpp"
#include "HashTableRegistry.hpp"
#include "LinkedListRegistry.hpp"
#include "MinHeapRegistry.hpp"
#include "SortedArrayRegistry.hpp"
#include "SortedListRegistry.hpp"

std::unique_ptr<ITargetRegistry> createCore(CoreType type)
{
    switch (type)
    {
        case CoreType::LinkedList:
            return std::make_unique<LinkedListRegistry>();
        case CoreType::SortedList:
            return std::make_unique<SortedListRegistry>();
        case CoreType::DynamicArray:
            return std::make_unique<DynamicArrayRegistry>();
        case CoreType::SortedArray:
            return std::make_unique<SortedArrayRegistry>();
        case CoreType::Bst:
            return std::make_unique<BstRegistry>();
        case CoreType::Avl:
            return std::make_unique<AvlRegistry>();
        case CoreType::MinHeap:
            return std::make_unique<MinHeapRegistry>();
        case CoreType::HashTable:
            return std::make_unique<HashTableRegistry>();
    }

    return nullptr;  // unreachable if every CoreType is handled above
}

std::optional<CoreType> coreTypeFromName(const std::string& name)
{
    if (name == "linked_list")
    {
        return CoreType::LinkedList;
    }
    if (name == "sorted_list")
    {
        return CoreType::SortedList;
    }
    if (name == "dynamic_array")
    {
        return CoreType::DynamicArray;
    }
    if (name == "sorted_array")
    {
        return CoreType::SortedArray;
    }
    if (name == "bst")
    {
        return CoreType::Bst;
    }
    if (name == "avl")
    {
        return CoreType::Avl;
    }
    if (name == "min_heap")
    {
        return CoreType::MinHeap;
    }
    if (name == "hash_table")
    {
        return CoreType::HashTable;
    }

    return std::nullopt;
}