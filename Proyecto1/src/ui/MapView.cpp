#include "MapView.hpp"

#include <QMouseEvent>
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

void MapView::mousePressEvent(QMouseEvent* event) {
    int cellX = event->pos().x() / kCellSize;
    int cellY = event->pos().y() / kCellSize;

    if (!Grid::inBounds(cellX, cellY)) {
        return;
    }
    if (grid_.at(cellX, cellY) != CellType::TowerSlot) {
        return;
    }

    const auto& positions = towerSlotPositions();
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (positions[i].first == cellX && positions[i].second == cellY) {
            emit slotClicked(static_cast<int>(i));
            return;
        }
    }
}