// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MainWindow.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include "CombatConstants.hpp"
#include "UpgradeDialog.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , simulation_(/*seed=*/12345, Config{}) {
  // Initialize central widget: HUD on top, the map in the middle, and the
  // per-slot panels below.
  QWidget* central = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(central);

  hud_ = new HudPanel(central);
  layout->addWidget(hud_);

  startWaveButton_ = new QPushButton("START WAVE", central);
  connect(startWaveButton_, &QPushButton::clicked, this
    , &MainWindow::onStartWave);
  layout->addWidget(startWaveButton_);

  hollowPurpleButton_ = new QPushButton(central);
  connect(hollowPurpleButton_, &QPushButton::clicked, this
    , &MainWindow::onHollowPurple);
  layout->addWidget(hollowPurpleButton_);

  // Same assets folder MapView loads its images from. If the file is
  // missing or can't be decoded, the effect just stays silent.
  const QString assetsPath = QDir::cleanPath(
    QCoreApplication::applicationDirPath() + "/../../assets");
  // QMediaPlayer plays through GStreamer, which reaches WSL's sound server
  // more reliably than QSoundEffect.
  hollowPurpleSound_ = new QMediaPlayer(this);
  connect(hollowPurpleSound_,
    QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, [this] {
      qWarning("Hollow Purple sound error: %s",
        qPrintable(hollowPurpleSound_->errorString()));
    });
  hollowPurpleSound_->setMedia(
    QUrl::fromLocalFile(assetsPath + "/sounds/hollow_purple.wav"));

  mapView_ = new MapView(central);
  mapView_->refresh(simulation_.state(), simulation_.slotManager());
  layout->addWidget(mapView_);

  slotGrid_ = new SlotGridView(central);
  layout->addWidget(slotGrid_);

  setCentralWidget(central);

  // A click on a tower-slot cell in the map opens the upgrade dialog for
  // that slot.
  connect(mapView_, &MapView::slotClicked, this, &MainWindow::onSlotClicked);

  hud_->refresh(simulation_.state());
  updateInteractivity();

  // Setup UI refresh timer to tick independently of simulation logic
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::onTick);
  timer_->start(REFRESH_INTERVAL_MS);

  setWindowTitle("Overflow: Algorithmic Tower Defense");
}

void MainWindow::onTick() {
  // Advance simulation engine using accumulated real-world elapsed time
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
  if (simulation_.state().phase != WavePhase::Construction) {
    return;  // towers stay fixed once combat starts (section 3.4)
  }

  // During Combat, tower structures must remain fixed.
  UpgradeDialog dialog(slotIndex, simulation_.economy(), this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto chosen = dialog.selectedCore();
  if (!chosen) {
    return;
  }

  if (!simulation_.purchaseCore(slotIndex, *chosen)) {
    return;  // shouldn't happen: unaffordable options are disabled in dialog
  }
  mapView_->refresh(simulation_.state(), simulation_.slotManager());
  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());
}

void MainWindow::onStartWave() {
  simulation_.startWave();
  updateInteractivity();
}

void MainWindow::onHollowPurple() {
  if (!simulation_.activateHollowPurple()) {
    return;  // shouldn't happen: the button is disabled when it can't
  }
  if (hollowPurpleSound_->mediaStatus() == QMediaPlayer::InvalidMedia
      || hollowPurpleSound_->mediaStatus() == QMediaPlayer::NoMedia) {
    qWarning("Hollow Purple sound can't be played (media status %d)",
      static_cast<int>(hollowPurpleSound_->mediaStatus()));
  }
  hollowPurpleSound_->setPosition(0);  // always from the start
  hollowPurpleSound_->play();          // lasts exactly the charge
  mapView_->refresh(simulation_.state(), simulation_.slotManager());
  hud_->refresh(simulation_.state());
  updateInteractivity();
}

void MainWindow::updateInteractivity() {
  const WorldState& state = simulation_.state();
  bool inConstruction = state.phase == WavePhase::Construction;
  startWaveButton_->setEnabled(inConstruction && !simulation_.over());
  slotGrid_->setEnabled(inConstruction);

  // Seconds left, rounded up, for the charge and cooldown labels.
  auto secondsLeft = [](int ticks) {
    return (ticks + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND;
  };
  QString label = QStringLiteral("虚式「茈」");
  if (state.hollow_purple_charge_left > 0) {
    label += QString(" \u2014 charging (%1 s)")
      .arg(secondsLeft(state.hollow_purple_charge_left));
  } else if (state.hollow_purple_cooldown_left > 0) {
    label += QString(" \u2014 ready in %1 s")
      .arg(secondsLeft(state.hollow_purple_cooldown_left));
  } else {
    label += QString(" \u2014 %1 credits").arg(HOLLOW_PURPLE_PRICE);
  }
  hollowPurpleButton_->setText(label);
  hollowPurpleButton_->setEnabled(simulation_.canActivateHollowPurple());
}

void MainWindow::handleMatchEnd() {
  timer_->stop();
  bool won = simulation_.economy().getLives() > 0;

  QMessageBox::information(this, won ? "Victory" : "Game Over"
    , won ? "You survived all 20 waves!" : "The base has fallen.");
}