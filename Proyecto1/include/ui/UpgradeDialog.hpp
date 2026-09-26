// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QDialog>
#include <optional>
#include "CorePrices.hpp"
#include "Economy.hpp"

class QListWidget;

// Section 5.3 — the single dialog that resolves the construction phase:
// the player picks a slot (already chosen by the caller), and this
// dialog shows every available structure with its price, greying out
// what the player can't afford.
class UpgradeDialog : public QDialog {
  Q_OBJECT

 public:
  UpgradeDialog(
    int slotIndex, const Economy& economy, QWidget* parent = nullptr);

  std::optional<CoreType> selectedCore() const;

 private:
  QListWidget* list_ = nullptr;
};
