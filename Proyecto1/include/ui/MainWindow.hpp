// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include "HudPanel.hpp"
#include "MapView.hpp"
#include "Simulation.hpp"
#include "SlotGridView.hpp"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

  private Q_SLOTS:
    void onTick();
    /**
     * @brief Opens the upgrade dialog for the clicked slot; on acceptance, 
     * purchases and installs the chosen core.
     */
    void onSlotClicked(int slotIndex);
    void onStartWave();

 private:
  // UI refresh rate, not the game's fixed tick
  static constexpr int REFRESH_INTERVAL_MS = 100;

  Simulation simulation_;
  HudPanel* hud_ = nullptr;
  MapView* mapView_ = nullptr;
  SlotGridView* slotGrid_ = nullptr;
  QPushButton* startWaveButton_ = nullptr;
  QTimer* timer_ = nullptr;

  /**
   * @brief Enables construction-phase controls (the button and the slot
   * grid) and disables them during combat, so towers stay fixed once a
   * wave starts.
   */
  void updateInteractivity();

  /**
   * @brief Shows a one-time Victory/Game Over message once the match
   * ends, and stops the refresh timer.
   */
  void handleMatchEnd();
};
