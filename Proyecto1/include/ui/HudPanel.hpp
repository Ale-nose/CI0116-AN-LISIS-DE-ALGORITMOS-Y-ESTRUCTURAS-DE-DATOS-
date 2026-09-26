// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QFrame>
#include <QLabel>

#include "Simulation.hpp"

/**
 * @brief Top panel showing credits, lives, the current
 * wave number, and the composition of that wave (Swarm/Wraith/Hive/
 * Decoy/Colossus counts).
 *
 * Purely a display: it never mutates Simulation, it only reads a
 * WorldState snapshot each time refresh() is called.
 */
class HudPanel : public QFrame {
  Q_OBJECT

 public:
  explicit HudPanel(QWidget* parent = nullptr);

  /**
   * @brief Updates the four labels from a fresh WorldState snapshot.
   * @param state Snapshot obtained from Simulation::state().
   */
  void refresh(const WorldState& state);

 private:
  QLabel* creditsLabel_;
  QLabel* livesLabel_;
  QLabel* waveLabel_;
  QLabel* compositionLabel_;

  /**
   * @brief Formats a wave's composition as "N Swarm, N Wraith, etc".
   * @param composition Composition to format.
   * @return Human-readable summary string.
   */
  static QString formatComposition(const WaveComposition& composition);
};
