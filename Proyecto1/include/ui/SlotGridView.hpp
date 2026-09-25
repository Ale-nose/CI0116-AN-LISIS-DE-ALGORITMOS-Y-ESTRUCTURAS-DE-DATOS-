#pragma once

#include <array>
#include <QWidget>
#include "SlotPanel.hpp"
#include "Slots.hpp"

// Section 5.2 — lays out one SlotPanel per slot in a 4-column grid.
// Also section 5.3's entry point: turns a click on any panel into a
// slotClicked(index) signal for the upgrade dialog to react to.
class SlotGridView : public QWidget
{
    Q_OBJECT

public:
    explicit SlotGridView(QWidget* parent = nullptr);

    void refresh(const SlotManager& manager);

signals:
    void slotClicked(int slotIndex);

private:
    std::array<SlotPanel*, SlotManager::kSlotCount> panels_{};
};