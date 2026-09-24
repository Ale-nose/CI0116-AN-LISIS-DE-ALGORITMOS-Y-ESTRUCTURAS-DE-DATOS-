#pragma once

#include <QMainWindow>
#include <QTimer>

#include "Simulation.hpp"
#include "SlotGridView.hpp"
#include "HudPanel.hpp"

class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  Simulation simulation_;
  HudPanel* hud_;            ///< Top panel: credits, lives, wave, next wave composition.
  SlotGridView* slotGrid_;   ///< Grid layout widget representing all tower slots.
  QTimer* timer_;            ///< Refresh timer driving real-time UI updates.

  static constexpr int REFRESH_INTERVAL_MS = 100; // UI refresh rate, not the game's fixed tick

 public:
  /**
   * @brief Constructs the main application window.
   * @param parent Optional pointer to the parent QWidget container.
   */
  explicit MainWindow(QWidget* parent = nullptr);

 private Q_SLOTS:
  /**
   * @brief Callback function executed automatically on every timer timeout.
   * Advances the game engine simulation and refreshes the UI slot view.
   */
  void onTick();
};
