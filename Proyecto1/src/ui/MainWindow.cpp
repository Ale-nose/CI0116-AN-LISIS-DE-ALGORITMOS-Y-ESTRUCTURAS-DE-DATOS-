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

  mapView_ = new MapView(central);
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
  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());
}

void MainWindow::onSlotClicked(int slotIndex) {
  UpgradeDialog dialog(slotIndex, simulation_.economy(), this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto chosen = dialog.selectedCore();
  if (!chosen) {
    return;
  }

  if (!simulation_.purchaseCore(slotIndex, *chosen)) {
    return;  // shouldn't happen: unaffordable options are disabled in the dialog
  }

  slotGrid_->refresh(simulation_.slotManager());
  hud_->refresh(simulation_.state());
}