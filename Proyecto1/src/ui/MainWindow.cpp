// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MainWindow.hpp"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>
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

  // Button style
  startWaveButton_->setStyleSheet(
      "QPushButton {"
      "  background-color: #c62828;"
      "  color: white;"
      "  font-weight: bold;"
      "  font-size: 14px;"
      "  border: 2px solid #8e0000;"
      "  border-radius: 5px;"
      "  padding: 8px 16px;"
      "}"
      "QPushButton:hover {"
      "  background-color: #e53935;"
      "}"
      "QPushButton:pressed {"
      "  background-color: #b71c1c;"
      "}"
      "QPushButton:disabled {"
      "  background-color: #555555;"
      "  color: #888888;"
      "  border: 1px solid #444444;"
      "}"
  );

  connect(startWaveButton_, &QPushButton::clicked, this
    , &MainWindow::onStartWave);
  layout->addWidget(startWaveButton_);

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

void MainWindow::updateInteractivity() {
  bool inConstruction = simulation_.state().phase == WavePhase::Construction;
  startWaveButton_->setEnabled(inConstruction && !simulation_.over());
  slotGrid_->setEnabled(inConstruction);
}

void MainWindow::handleMatchEnd() {
  timer_->stop();
  bool won = simulation_.economy().getLives() > 0;

  QMessageBox::information(this, won ? "Victory" : "Game Over"
    , won ? "You survived all 20 waves!" : "The base has fallen.");
}
