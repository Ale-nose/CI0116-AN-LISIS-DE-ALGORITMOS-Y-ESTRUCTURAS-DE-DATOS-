#include "MainWindow.hpp"

#include <QVBoxLayout>
#include <QWidget>
#include "UpgradeDialog.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , simulation_(/*seed=*/12345, Config{}) {
  // Section 5.4 groundwork: just the credits line for now, the rest of
  // the top panel (lives, wave number, next wave) belongs to Topic 5.4.
  creditsLabel_ = new QLabel(this);

  mapView_ = new MapView(this);
  slotGrid_ = new SlotGridView(this);

  // Stack the map (5.1) above the per-slot panels (5.2), with the
  // credits line on top, all inside one central widget.
  auto* central = new QWidget(this);
  auto* layout = new QVBoxLayout(central);
  layout->addWidget(creditsLabel_);
  layout->addWidget(mapView_);
  layout->addWidget(slotGrid_);
  setCentralWidget(central);

  // Section 5.3 — a click on a tower-slot cell in the map opens the
  // upgrade dialog for that slot.
  connect(mapView_, &MapView::slotClicked, this, &MainWindow::onSlotClicked);

  // Setup UI refresh timer to tick independently of simulation logic
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::onTick);
  timer_->start(REFRESH_INTERVAL_MS);

  refreshCreditsLabel();
  setWindowTitle("Overflow: Algorithmic Tower Defense");
}

void MainWindow::onTick() {
  // Advance simulation engine using accumulated real-world elapsed time
  simulation_.advance(REFRESH_INTERVAL_MS);
  slotGrid_->refresh(simulation_.slotManager());
}

void MainWindow::onSlotClicked(int slotIndex) {
  UpgradeDialog dialog(slotIndex, economy_, this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto chosen = dialog.selectedCore();
  if (!chosen) {
    return;
  }

  if (!economy_.buyCore(*chosen)) {
    return;  // shouldn't happen: unaffordable options are disabled in the dialog
  }

  simulation_.slotManager().installCore(slotIndex, *chosen);
  slotGrid_->refresh(simulation_.slotManager());
  refreshCreditsLabel();
}

void MainWindow::refreshCreditsLabel() {
  creditsLabel_->setText(QString("Créditos: %1").arg(economy_.getCredits()));
}