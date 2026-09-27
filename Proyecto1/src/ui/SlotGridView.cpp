// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "SlotGridView.hpp"
#include <QGridLayout>
#include <algorithm>
#include "Tower.hpp"

SlotGridView::SlotGridView(QWidget* parent) : QWidget(parent) {
  setObjectName("slotGrid");

  auto* layout = new QGridLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setHorizontalSpacing(8);
  layout->setVerticalSpacing(8);

  constexpr int kColumns = 4;

  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    auto* panel = new SlotPanel(this);
    panels_[i] = panel;

    const int row = i / kColumns;
    const int col = i % kColumns;

    layout->addWidget(panel, row, col);
  }

  setStyleSheet(
    "#slotGrid {"
    "background-color: #172019;"
    "border: 2px solid #344638;"
    "border-radius: 10px;"
    "}");
}

void SlotGridView::refresh(const SlotManager& manager) {
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    if (!manager.hasTower(i)) {
      panels_[i]->showEmpty();
      continue;
    }

    const Tower* tower = manager.towerAt(i);
    auto type = manager.installedType(i);

    const int pending = static_cast<int>(tower->pendingCount());
    const int lagPercent = std::min(100, pending * 100 / MAX_EXPECTED_PENDING);

    panels_[i]->showOccupied(*type, tower->registrySize(), lagPercent);
  }
}
