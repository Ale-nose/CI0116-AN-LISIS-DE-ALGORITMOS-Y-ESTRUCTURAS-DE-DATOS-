#include <QVBoxLayout>

#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , simulation_(/*seed=*/12345, Config{}) {
  // Initialize grid container and assign it as central window widget
  QWidget* central = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(central);

  hud_ = new HudPanel(central);
  layout->addWidget(hud_);

  slotGrid_ = new SlotGridView(central);
  layout->addWidget(slotGrid_);

  setCentralWidget(central);

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
  slotGrid_->refresh(simulation_.slots());
  hud_->refresh(simulation_.state());
}
