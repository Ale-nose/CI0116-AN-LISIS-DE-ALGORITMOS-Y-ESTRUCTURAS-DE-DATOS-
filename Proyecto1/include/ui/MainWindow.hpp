#pragma once

#include <QMainWindow>
#include <QTimer>
#include "HudPanel.hpp"
#include "MapView.hpp"
#include "Simulation.hpp"
#include "SlotGridView.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onTick();

    // Section 5.3 — opens the upgrade dialog for the clicked slot; on
    // acceptance, purchases and installs the chosen core.
    void onSlotClicked(int slotIndex);

private:
    static constexpr int REFRESH_INTERVAL_MS = 100;  // UI refresh rate, not the game's fixed tick

    Simulation simulation_;
    HudPanel* hud_ = nullptr;
    MapView* mapView_ = nullptr;
    SlotGridView* slotGrid_ = nullptr;
    QTimer* timer_ = nullptr;
};