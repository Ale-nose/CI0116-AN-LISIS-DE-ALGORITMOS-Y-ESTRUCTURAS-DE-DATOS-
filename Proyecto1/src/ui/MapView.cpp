#include "MapView.hpp"

#include <QPainter>
#include "MapBuilder.hpp"

MapView::MapView(QWidget* parent) : QWidget(parent) {
    buildDefaultMap(grid_);
    setFixedSize(GRID_WIDTH * kCellSize, GRID_HEIGHT * kCellSize);
}

void MapView::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            QRect cellRect(x * kCellSize, y * kCellSize, kCellSize, kCellSize);

            switch (grid_.at(x, y)) {
            case CellType::Path:
                painter.fillRect(cellRect, Qt::lightGray);
                break;
            case CellType::Entrance:
                painter.fillRect(cellRect, Qt::green);
                break;
            case CellType::Exit:
                painter.fillRect(cellRect, Qt::red);
                break;
            case CellType::TowerSlot:
                painter.fillRect(cellRect, Qt::blue);
                break;
            default:
                painter.fillRect(cellRect, Qt::white);
                break;
            }

            painter.setPen(Qt::black);
            painter.drawRect(cellRect);
        }
    }
}
