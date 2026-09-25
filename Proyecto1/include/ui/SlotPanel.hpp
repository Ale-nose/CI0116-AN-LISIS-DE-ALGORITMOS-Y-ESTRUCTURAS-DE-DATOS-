#pragma once

#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include <QString>
#include "CorePrices.hpp"

// Section 5.2 — one slot's on-screen indicators: installed structure,
// current registry size, and a lag bar proportional to the pending
// maintenance queue. Also the click target that opens the upgrade
// dialog (5.3) for this slot.
class SlotPanel : public QFrame
{
    Q_OBJECT

public:
    explicit SlotPanel(QWidget* parent = nullptr);

    void showEmpty();
    void showOccupied(CoreType type, std::size_t enemiesTracked, int lagPercent);

signals:
    // Emitted on any click on this panel. SlotGridView knows which
    // index this panel is and re-emits slotClicked(index) from it.
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    static QString coreTypeName(CoreType type);

    QLabel* structureLabel_ = nullptr;
    QLabel* sizeLabel_ = nullptr;
    QProgressBar* lagBar_ = nullptr;
};