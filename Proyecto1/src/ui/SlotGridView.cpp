#include "SlotGridView.hpp"

#include <algorithm>
#include <QGridLayout>

#include "Tower.hpp"

SlotGridView::SlotGridView(QWidget* parent) : QWidget(parent) {
  auto* layout = new QGridLayout(this);
  layout->setSpacing(10);

  constexpr int kColumns = 4;

  // Instantiate and place slot panels in a 4-column grid layout
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    auto* panel = new SlotPanel(this);
    panels_[i] = panel;

    int row = i / kColumns;
    int col = i % kColumns;
    layout->addWidget(panel, row, col);
  }

  setLayout(layout);
}

void SlotGridView::refresh(const SlotManager& slots) {
  for (int i = 0; i < SlotManager::kSlotCount; i++) {
    // Reset panel view if no tower/core is currently installed
    if (!slots.hasTower(i)) {
      panels_[i]->showEmpty();
      continue;
    }

    const Tower* tower = slots.towerAt(i);
    auto type = slots.installedType(i);

    int lagPercent = std::min(100, static_cast<int>(tower->pendingCount()) 
    * 100 / MAX_EXPECTED_PENDING);  // maintenance backlog percentage
  
    // Update occupied slot panel with registry metrics
    panels_[i]->showOccupied(*type, tower->registrySize(), lagPercent);
  }
}
