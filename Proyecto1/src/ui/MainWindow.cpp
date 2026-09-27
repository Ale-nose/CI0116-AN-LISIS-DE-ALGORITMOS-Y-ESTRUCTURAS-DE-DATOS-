// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include "UpgradeDialog.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , simulation_(/*seed=*/12345, Config{}) {
  // Initialize central widget: HUD (5.4) on top, the map (5.1) in the
  // middle, and the per-slot panels (5.2) below.
  QWidget* central = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(central);

  hud_ = new HudPanel(central);
  layout->addWidget(hud_);

  // TODO: Add a START WAVE button for the Construction phase. Clicking it
  // must call Simulation::startWave() and disable the button during Combat.

  mapView_ = new MapView(central);
  mapView_->refresh(simulation_.state(), simulation_.slotManager());
  layout->addWidget(mapView_);

  slotGrid_ = new SlotGridView(central);
  layout->addWidget(slotGrid_);

  setCentralWidget(central);

  // Section 5.3 — a click on a tower-slot cell in the map opens the
  // upgrade dialog for that slot.
  connect(mapView_, &MapView::slotClicked, this, &MainWindow::onSlotClicked);

  hud_->refresh(simulation_.state());

  // Setup UI refresh timer to tick independently of simulation logic
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::onTick);
  timer_->start(REFRESH_INTERVAL_MS);

  setWindowTitle("Overflow: Algorithmic Tower Defense");
}

void MainWindow::onTick() {
  // Advance simulation engine using accumulated real-world elapsed time
  simulation_.advance(REFRESH_INTERVAL_MS);
  // TODO: Check simulation_.over(). If the match ended, stop the timer and
  // show a final Victory or Game Over message to the player.
  mapView_->refresh(simulation_.state(), simulation_.slotManager());
  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());
}

void MainWindow::onSlotClicked(int slotIndex) {
  // TODO: Allow tower purchases and upgrades only during Construction.
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
