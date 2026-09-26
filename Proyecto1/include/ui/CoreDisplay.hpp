// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QString>
#include "CorePrices.hpp"

// Human-readable name for a CoreType, for the upgrade dialog (5.3) and
// any other UI that needs to show a structure's name to the player.
inline QString coreDisplayName(CoreType type) {
  switch (type) {
    case CoreType::LinkedList:
      return QStringLiteral("Lista enlazada");
    case CoreType::SortedList:
      return QStringLiteral("Lista ordenada");
    case CoreType::DynamicArray:
      return QStringLiteral("Arreglo dinámico");
    case CoreType::SortedArray:
      return QStringLiteral("Arreglo ordenado");
    case CoreType::Bst:
      return QStringLiteral("ABB sin balanceo");
    case CoreType::Avl:
      return QStringLiteral("AVL");
    case CoreType::MinHeap:
      return QStringLiteral("Montículo mínimo");
    case CoreType::HashTable:
      return QStringLiteral("Tabla hash");
  }
  return QStringLiteral("Desconocido");
}
