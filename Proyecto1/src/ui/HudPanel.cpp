// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "HudPanel.hpp"

#include <QHBoxLayout>

namespace {
// Display names, in the same order as the EnemyCategory enum.
const char* kCategoryNames[] = {"Swarm", "Wraith", "Hive", "Decoy", "Colossus"};
}  // namespace

HudPanel::HudPanel(QWidget* parent) : QFrame(parent) {
  setFrameShape(QFrame::StyledPanel);

  creditsLabel_ = new QLabel(this);
  livesLabel_ = new QLabel(this);
  waveLabel_ = new QLabel(this);
  compositionLabel_ = new QLabel(this);

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(creditsLabel_);
  layout->addWidget(livesLabel_);
  layout->addWidget(waveLabel_);
  layout->addWidget(compositionLabel_);
  layout->addStretch();  // keeps the four labels left-aligned
}

void HudPanel::refresh(const WorldState& state) {
  creditsLabel_->setText(QString("Credits: %1").arg(state.credits));
  livesLabel_->setText(QString("Lives: %1").arg(state.lives));
  waveLabel_->setText(
    QString("Wave: %1/%2").arg(state.current_wave).arg(TOTAL_WAVES));
  compositionLabel_->setText(
    "Next wave: " + formatComposition(state.next_wave_composition));
}

QString HudPanel::formatComposition(const WaveComposition& composition) {
  QString text;
  for (std::size_t i = 0; i < kCategoryCount; ++i) {
    if (i > 0) {
      text += ", ";
    }
    text +=
      QString("%1 %2").arg(composition.perCategory[i]).arg(kCategoryNames[i]);
  }
  return text;
}
