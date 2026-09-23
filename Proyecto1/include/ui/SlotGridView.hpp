#pragma once

#include <QWidget>
#include <array>

#include "SlotPanel.hpp"
#include "Slots.hpp"

/// Maximum expected pending maintenance operations before lag bar hits 100%.
constexpr int MAX_EXPECTED_PENDING = 20;

class SlotGridView : public QWidget {
  Q_OBJECT

 private:
  /// Array of slot panel widgets mapping to each tower slot.
  std::array<SlotPanel*, SlotManager::kSlotCount> panels_;

 public:
  /**
   * @brief Constructs the slot grid view container.
   * @param parent Optional pointer to the parent QWidget container.
   */
  explicit SlotGridView(QWidget* parent = nullptr);

  /**
   * @brief Refreshes all slot panel displays using the current SlotManager 
   * state.
   * @param slots Const reference to the simulation's SlotManager instance.
   */
  void refresh(const SlotManager& slots);
};
