#pragma once

#include <QWidget>
#include "Grid.hpp"

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Grid grid_;
    static constexpr int kCellSize = 30;  // pixels per cell
};
