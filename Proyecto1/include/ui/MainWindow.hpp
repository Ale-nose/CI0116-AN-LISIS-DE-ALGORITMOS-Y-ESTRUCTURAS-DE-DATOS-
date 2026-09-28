// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <cstddef>
#include <cstdint>
#include "HudPanel.hpp"
#include "MapView.hpp"
#include "Replay.hpp"
#include "Simulation.hpp"
#include "SlotGridView.hpp"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(bool challengeMode,
      const ReplayData* replayData = nullptr, QWidget* parent = nullptr);

  private Q_SLOTS:
  void onTick();

  /**
   * @brief Opens the upgrade dialog for the clicked slot and installs the
   * selected core when the purchase succeeds.
   */
  void onSlotClicked(int slotIndex);
  void onStartWave();
  void onShowChallengeLeaderboard();

  /**
   * @brief Activates Hollow Purple when the ability can be purchased.
   */
  void onHollowPurple();

 private:
  // UI refresh rate, not the game's fixed tick.
  static constexpr int REFRESH_INTERVAL_MS = 100;

  bool challengeMode_ = false;
  bool replayMode_ = false;
  ReplayData replayData_;
  std::size_t replayDecisionIndex_ = 0;
  std::uint32_t seed_ = 0;
  QString challengePlayer_;

  Simulation simulation_;

  HudPanel* hud_ = nullptr;
  MapView* mapView_ = nullptr;
  SlotGridView* slotGrid_ = nullptr;
  QPushButton* startWaveButton_ = nullptr;
  QPushButton* hollowPurpleButton_ = nullptr;  ///< Special ability.
  QMediaPlayer* hollowPurpleSound_ = nullptr;  ///< Plays during its charge.
  QTimer* timer_ = nullptr;
  QPushButton* leaderboardButton_ = nullptr;

  /**
   * @brief Enables construction controls and updates Hollow Purple state.
   */
  void updateInteractivity();

  /**
   * @brief Stops the timer and shows the final match result.
   */
  void handleMatchEnd();

  bool applyReplayWave();
};
