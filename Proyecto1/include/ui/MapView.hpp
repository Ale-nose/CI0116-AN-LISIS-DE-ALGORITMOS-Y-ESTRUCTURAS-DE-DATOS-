// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QWidget>
#include <QPixmap>
#include "Grid.hpp"

class MapView : public QWidget {
  Q_OBJECT

 public:
  explicit MapView(QWidget* parent = nullptr);

Q_SIGNALS:
  // Section 5.3's trigger: emitted when the player clicks a cell that
  // is one of the 8 tower slots. slotIndex matches SlotManager's
  // 0..7 indexing (see MapBuilder::towerSlotPositions()).
  void slotClicked(int slotIndex);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

 private:
  Grid grid_;
  QPixmap mapPixmap_;
  static constexpr int kCellSize = 30;  // pixels per cell
};
