#pragma once

#include <QWidget>
#include "Grid.hpp"

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

signals:
    // Section 5.3's trigger: emitted when the player clicks a cell that
    // is one of the 8 tower slots. slotIndex matches SlotManager's
    // 0..7 indexing (see MapBuilder::towerSlotPositions()).
    void slotClicked(int slotIndex);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    Grid grid_;
    static constexpr int kCellSize = 30;  // pixels per cell
};