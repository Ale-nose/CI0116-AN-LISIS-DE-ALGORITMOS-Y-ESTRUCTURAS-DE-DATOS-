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
   * @note Parameter named `manager`, not `slots` — Qt defines `slots` as
   * a macro, which breaks a parameter with that exact name in any file
   * that also includes Qt headers (this one does).
   * @param manager Const reference to the simulation's SlotManager instance.
   */
  void refresh(const SlotManager& manager);
};