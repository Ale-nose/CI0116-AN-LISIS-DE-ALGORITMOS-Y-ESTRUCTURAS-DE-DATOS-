// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <array>
#include <optional>
#include "Grid.hpp"
#include "Simulation.hpp"

class MapView : public QWidget {
  Q_OBJECT

 public:
  explicit MapView(QWidget* parent = nullptr);

  void refresh(const WorldState& state, const SlotManager& slotManager);

Q_SIGNALS:
  void slotClicked(int slotIndex);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

 private:
  /**
   * @brief Draws the Hollow Purple chant over the map while it charges:
   * the four lines appear one by one, evenly spread over the charge, on
   * a translucent purple veil.
   * @param painter Painter already drawing this widget.
   */
  void drawHollowPurpleChant(QPainter& painter);

  static constexpr int kChantFontPointSize = 30;  ///< Chant text size.
  static constexpr int kChantVeilAlpha = 150;     ///< Veil opacity 0..255.

  Grid grid_;

  QPixmap mapPixmap_;

  std::array<QPixmap, 8> towerSprites_;
  std::array<QPixmap, kCategoryCount> enemySprites_;

  std::array<
    std::optional<CoreType>,
    SlotManager::kSlotCount>
    installedCores_{};

  WorldState worldState_;

  static constexpr int kCellSize = 30;
};
