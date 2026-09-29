// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MainWindow.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <string>
#include "ChallengeLeaderboard.hpp"
#include "ChallengeLeaderboardDialog.hpp"
#include "ChallengeMode.hpp"
#include "CombatConstants.hpp"
#include "Replay.hpp"
#include "UpgradeDialog.hpp"

namespace {

std::string challengeScoresPath() {
  const QString path = QDir::cleanPath(
    QCoreApplication::applicationDirPath()
    + "/../../report/challenge_scores.csv");

  return path.toStdString();
}

constexpr std::uint32_t DEFAULT_UI_SEED = 12345;

std::uint32_t launchSeed(
    bool challengeMode,
    const ReplayData* replayData) {
  if (replayData != nullptr) {
    return replayData->seed;
  }

  return challengeMode ? CHALLENGE_SEED : DEFAULT_UI_SEED;
}

std::string replayOutputPath() {
  const QString path = QDir::cleanPath(
    QCoreApplication::applicationDirPath()
    + "/../../report/last_replay.csv");

  return path.toStdString();
}

}  // namespace

MainWindow::MainWindow(
    bool challengeMode,
    const ReplayData* replayData,
    QWidget* parent)
    : QMainWindow(parent),
    challengeMode_(challengeMode),
    replayMode_(replayData != nullptr),
    replayData_(replayData ? *replayData : ReplayData{}),
    seed_(launchSeed(challengeMode, replayData)),
    simulation_(seed_, Config{}) {
  if (!replayMode_) {
    replayData_.seed = seed_;
  }

  QWidget* central = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(central);

  if (challengeMode_) {
    bool accepted = false;

    challengePlayer_ = QInputDialog::getText(
      this,
      "Challenge Mode",
      "Player name:",
      QLineEdit::Normal,
      "",
      &accepted);

    if (!accepted) {
      challengePlayer_ = "Player";
    }

    challengePlayer_ = challengePlayer_.trimmed();
    challengePlayer_.replace(',', ' ');
    challengePlayer_.replace('\n', ' ');
    challengePlayer_.replace('\r', ' ');
    challengePlayer_ = challengePlayer_.simplified();

    if (challengePlayer_.isEmpty()) {
      challengePlayer_ = "Player";
    }
  }

  auto* topLayout = new QHBoxLayout;
  topLayout->setContentsMargins(0, 0, 0, 0);
  topLayout->setSpacing(8);

  hud_ = new HudPanel(central);
  hud_->setChallengeMode(challengeMode_);
  topLayout->addWidget(hud_, 1);

  leaderboardButton_ = new QPushButton("LEADERBOARD", central);
  leaderboardButton_->setObjectName("leaderboardButton");
  leaderboardButton_->setMinimumHeight(52);
  leaderboardButton_->setVisible(challengeMode_);

  leaderboardButton_->setStyleSheet(
    "#leaderboardButton {"
    "background-color: #49355c;"
    "color: #e5c8ff;"
    "border: 1px solid #795899;"
    "border-radius: 7px;"
    "padding: 8px 14px;"
    "font-size: 13px;"
    "font-weight: bold;"
    "}"
    "#leaderboardButton:hover {"
    "background-color: #5b4172;"
    "}"
    "#leaderboardButton:pressed {"
    "background-color: #382846;"
    "}");

  connect(
    leaderboardButton_,
    &QPushButton::clicked,
    this,
    &MainWindow::onShowChallengeLeaderboard);

  topLayout->addWidget(leaderboardButton_);
  layout->addLayout(topLayout);

  startWaveButton_ = new QPushButton("START WAVE", central);

  startWaveButton_->setStyleSheet(
    "QPushButton {"
    "background-color: #c62828;"
    "color: white;"
    "font-weight: bold;"
    "font-size: 14px;"
    "border: 2px solid #8e0000;"
    "border-radius: 5px;"
    "padding: 8px 16px;"
    "}"
    "QPushButton:hover {"
    "background-color: #e53935;"
    "}"
    "QPushButton:pressed {"
    "background-color: #b71c1c;"
    "}"
    "QPushButton:disabled {"
    "background-color: #555555;"
    "color: #888888;"
    "border: 1px solid #444444;"
    "}");

  connect(
    startWaveButton_,
    &QPushButton::clicked,
    this,
    &MainWindow::onStartWave);

  layout->addWidget(startWaveButton_);

  hollowPurpleButton_ = new QPushButton(central);

  connect(
    hollowPurpleButton_,
    &QPushButton::clicked,
    this,
    &MainWindow::onHollowPurple);

  layout->addWidget(hollowPurpleButton_);

  const QString assetsPath = QDir::cleanPath(
    QCoreApplication::applicationDirPath()
    + "/../../assets");

  hollowPurpleSound_ = new QMediaPlayer(this);

  connect(
    hollowPurpleSound_,
    QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, [this] {
      qWarning(
        "Hollow Purple sound error: %s",
        qPrintable(hollowPurpleSound_->errorString()));
    });

  hollowPurpleSound_->setMedia(
    QUrl::fromLocalFile(
      assetsPath + "/sounds/hollow_purple.wav"));

  mapView_ = new MapView(central);
  mapView_->refresh(
    simulation_.state(),
    simulation_.slotManager());

  layout->addWidget(mapView_);

  slotGrid_ = new SlotGridView(central);
  layout->addWidget(slotGrid_);

  setCentralWidget(central);

  connect(
    mapView_,
    &MapView::slotClicked,
    this,
    &MainWindow::onSlotClicked);

  hud_->refresh(simulation_.state());
  updateInteractivity();

  timer_ = new QTimer(this);

  connect(
    timer_,
    &QTimer::timeout,
    this,
    &MainWindow::onTick);

  timer_->start(REFRESH_INTERVAL_MS);

  if (replayMode_) {
    setWindowTitle("Overflow: Algorithmic Tower Defense - REPLAY");
  } else {
    setWindowTitle("Overflow: Algorithmic Tower Defense");
  }
}

bool MainWindow::applyReplayWave() {
  if (!replayMode_ || !simulation_.inConstruction()) {
    return true;
  }

  const int wave = simulation_.state().current_wave;

  while (replayDecisionIndex_ < replayData_.decisions.size()) {
    const PlayerDecision& decision =
      replayData_.decisions[replayDecisionIndex_];

    if (decision.wave > wave) {
      break;
    }

    if (decision.wave < wave) {
      return false;
    }

    if (!simulation_.purchaseCore(decision.slot, decision.core)) {
      return false;
    }

    ++replayDecisionIndex_;
  }

  simulation_.startWave();
  return true;
}

void MainWindow::onTick() {
  if (replayMode_ && simulation_.inConstruction()) {
    if (!applyReplayWave()) {
      timer_->stop();

      QMessageBox::critical(
        this,
        "Replay Error",
        "The replay diverged from the recorded match.");

      return;
    }
  }

  simulation_.advance(REFRESH_INTERVAL_MS);

  mapView_->refresh(simulation_.state(), simulation_.slotManager());

  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());

  updateInteractivity();

  if (simulation_.over()) {
    handleMatchEnd();
  }
}

void MainWindow::onSlotClicked(int slotIndex) {
  if (replayMode_) {
    return;
  }

  if (simulation_.state().phase != WavePhase::Construction) {
    return;
  }

  UpgradeDialog dialog(
    slotIndex,
    simulation_.economy(),
    this);

  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto chosen = dialog.selectedCore();

  if (!chosen) {
    return;
  }

  if (!simulation_.purchaseCore(slotIndex, *chosen)) {
    return;
  }

  PlayerDecision decision;
  decision.wave = simulation_.state().current_wave;
  decision.slot = slotIndex;
  decision.core = *chosen;

  replayData_.decisions.push_back(decision);

  mapView_->refresh(simulation_.state(), simulation_.slotManager());

  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());
}

void MainWindow::onStartWave() {
  if (replayMode_) {
    return;
  }

  simulation_.startWave();
  updateInteractivity();
}

void MainWindow::onHollowPurple() {
  if (replayMode_) {
    return;
  }

  if (!simulation_.activateHollowPurple()) {
    return;
  }

  if (hollowPurpleSound_->mediaStatus()
    == QMediaPlayer::InvalidMedia
    || hollowPurpleSound_->mediaStatus()
    == QMediaPlayer::NoMedia) {
    qWarning("Hollow Purple sound can't be played (media status %d)",
      static_cast<int>(hollowPurpleSound_->mediaStatus()));
  }

  hollowPurpleSound_->setPosition(0);
  hollowPurpleSound_->play();

  mapView_->refresh(simulation_.state(), simulation_.slotManager());

  hud_->refresh(simulation_.state());
  updateInteractivity();
}

void MainWindow::updateInteractivity() {
  const WorldState& state = simulation_.state();

  const bool inConstruction =
    state.phase == WavePhase::Construction;

  const bool canInteract = inConstruction && !simulation_.over()
    && !replayMode_;

  startWaveButton_->setEnabled(canInteract);
  slotGrid_->setEnabled(canInteract);

  auto secondsLeft = [](int ticks) {
    return (ticks + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND;
  };

  QString label = QStringLiteral("虚式「茈」");

  if (state.hollow_purple_charge_left > 0) {
    label += QString(" — charging (%1 s)").arg(
      secondsLeft(state.hollow_purple_charge_left));
  } else if (state.hollow_purple_cooldown_left > 0) {
    label += QString(" — ready in %1 s").arg(
      secondsLeft(state.hollow_purple_cooldown_left));
  } else {
    label += QString(" — %1 credits").arg(HOLLOW_PURPLE_PRICE);
  }

  hollowPurpleButton_->setText(label);

  hollowPurpleButton_->setEnabled(!replayMode_
    && simulation_.canActivateHollowPurple());
}

void MainWindow::handleMatchEnd() {
  timer_->stop();

  if (!replayMode_) {
    if (!saveReplay(replayOutputPath(), replayData_)) {
      QMessageBox::warning(
        this,
        "Replay",
        "Could not save the replay file.");
    }
  }

  if (challengeMode_) {
    ChallengeResult result;

    result.player = challengePlayer_.toStdString();
    result.wavesCompleted = simulation_.stats().waves_completed;
    result.livesRemaining = simulation_.state().lives;
    result.creditsRemaining = simulation_.state().credits;

    ChallengeLeaderboard leaderboard(challengeScoresPath());

    leaderboard.load();
    leaderboard.addResult(result);

    if (!leaderboard.save()) {
      QMessageBox::warning(
        this,
        "Challenge Mode",
        "Could not save the challenge leaderboard.");
    }
  }

  const bool won = simulation_.state().lives > 0;

  QMessageBox::information(this,
    won ? "Victory" : "Game Over",
    won ? "You survived all 20 waves!" : "The base has fallen.");

  if (challengeMode_) {
    onShowChallengeLeaderboard();
  }
}

void MainWindow::onShowChallengeLeaderboard() {
  ChallengeLeaderboard leaderboard(challengeScoresPath());

  leaderboard.load();

  ChallengeLeaderboardDialog dialog(leaderboard.results(), this);

  dialog.exec();
}
