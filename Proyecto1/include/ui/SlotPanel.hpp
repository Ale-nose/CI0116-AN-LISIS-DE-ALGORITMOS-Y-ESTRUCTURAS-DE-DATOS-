#pragma once

#include <QFrame>
#include <QLabel>
#include <QProgressBar>

#include "CorePrices.hpp"

class SlotPanel : public QFrame {
  Q_OBJECT

 private:
  QLabel* structureLabel_;  ///< Displays installed structure/core name or 'Empty'.
  QLabel* sizeLabel_;       ///< Displays total count of active tracked enemies.
  QProgressBar* lagBar_;    ///< Progress bar showing queued maintenance load percentage.

  /**
   * @brief Converts a CoreType enumeration value to string name.
   * @param type CoreType enum value.
   * @return QString containing the formatted name of the structure core type.
   */
  static QString coreTypeName(CoreType type);

 public:
  /**
   * @brief Constructs a single slot panel widget.
   * @param parent Optional pointer to the parent QWidget container.
   */
  explicit SlotPanel(QWidget* parent = nullptr);

  /**
   * @brief Configures panel UI to display an unoccupied/empty state.
   */
  void showEmpty();

  /**
   * @brief Updates panel UI with metrics for an occupied tower slot.
   * @param type Concrete CoreType structure installed in the slot.
   * @param enemiesTracked Current count of enemies stored inside the core 
   * registry.
   * @param lagPercent Calculated maintenance lag load percentage.
   */
  void showOccupied(CoreType type, std::size_t enemiesTracked, int lagPercent);
};
