#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include "Economy.hpp"
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
    // acceptance, charges Economy and installs the chosen core.
    void onSlotClicked(int slotIndex);

private:
    static constexpr int REFRESH_INTERVAL_MS = MS_PER_TICK;

    void refreshCreditsLabel();

    Simulation simulation_;
    Economy economy_;
    MapView* mapView_ = nullptr;
    SlotGridView* slotGrid_ = nullptr;
    QLabel* creditsLabel_ = nullptr;
    QTimer* timer_ = nullptr;
};