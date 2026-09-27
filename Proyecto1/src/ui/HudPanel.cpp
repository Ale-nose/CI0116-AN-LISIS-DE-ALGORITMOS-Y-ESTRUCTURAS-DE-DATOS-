// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "HudPanel.hpp"
#include <QHBoxLayout>
#include <QFontMetrics>

namespace {

const char* kCategoryNames[] = {
  "Swarm",
  "Wraith",
  "Hive",
  "Decoy",
  "Colossus"
};

}  // namespace

HudPanel::HudPanel(QWidget* parent) : QFrame(parent) {
  setObjectName("hudPanel");
  setMinimumHeight(72);

  creditsLabel_ = new QLabel(this);
  livesLabel_ = new QLabel(this);
  waveLabel_ = new QLabel(this);
  compositionLabel_ = new QLabel(this);

  creditsLabel_->setObjectName("creditsLabel");
  livesLabel_->setObjectName("livesLabel");
  waveLabel_->setObjectName("waveLabel");
  compositionLabel_->setObjectName("compositionLabel");

  creditsLabel_->setAlignment(Qt::AlignCenter);
  livesLabel_->setAlignment(Qt::AlignCenter);
  waveLabel_->setAlignment(Qt::AlignCenter);
  compositionLabel_->setAlignment(Qt::AlignCenter);

  auto* layout = new QHBoxLayout(this);

  layout->setContentsMargins(12, 10, 12, 10);
  layout->setSpacing(10);

  layout->addWidget(creditsLabel_);
  layout->addWidget(livesLabel_);
  layout->addWidget(waveLabel_);
  layout->addWidget(compositionLabel_, 1);

  setStyleSheet(
    "#hudPanel {"
    "background-color: #1b2a1d;"
    "border: 2px solid #3d5740;"
    "border-radius: 10px;"
    "}"
    "#creditsLabel {"
    "background-color: #4b3b1f;"
    "color: #f2cf66;"
    "border: 1px solid #80672f;"
    "border-radius: 7px;"
    "padding: 8px;"
    "font-size: 14px;"
    "font-weight: bold;"
    "}"
    "#livesLabel {"
    "background-color: #4a2525;"
    "color: #ef8989;"
    "border: 1px solid #804040;"
    "border-radius: 7px;"
    "padding: 8px;"
    "font-size: 14px;"
    "font-weight: bold;"
    "}"
    "#waveLabel {"
    "background-color: #23394a;"
    "color: #8fc7eb;"
    "border: 1px solid #40627a;"
    "border-radius: 7px;"
    "padding: 8px;"
    "font-size: 14px;"
    "font-weight: bold;"
    "}"
    "#compositionLabel {"
    "background-color: #26392a;"
    "color: #dce8dc;"
    "border: 1px solid #4a674e;"
    "border-radius: 7px;"
    "padding: 8px;"
    "font-size: 13px;"
    "}"
  );
}

void HudPanel::refresh(const WorldState& state) {
  const QString creditsText = QString("CREDITS  %1").arg(state.credits);
  const QString livesText = QString("LIVES  %1").arg(state.lives);
  const QString waveText = QString("WAVE  %1 / %2").arg(state.current_wave)
    .arg(TOTAL_WAVES);
  const QString compositionText = "NEXT WAVE   "
    + formatComposition(state.next_wave_composition);

  creditsLabel_->setText(creditsText);
  livesLabel_->setText(livesText);
  waveLabel_->setText(waveText);
  compositionLabel_->setText(compositionText);

  const int padding = 28;

  creditsLabel_->setMinimumWidth(
    creditsLabel_->fontMetrics().horizontalAdvance(creditsText) + padding);

  livesLabel_->setMinimumWidth(
    livesLabel_->fontMetrics().horizontalAdvance(livesText) + padding);

  waveLabel_->setMinimumWidth(
    waveLabel_->fontMetrics().horizontalAdvance(waveText) + padding);

  compositionLabel_->setMinimumWidth(
    compositionLabel_->fontMetrics().horizontalAdvance(compositionText)
    + padding);
}

QString HudPanel::formatComposition(const WaveComposition& composition) {
  QString text;

  for (std::size_t i = 0; i < kCategoryCount; ++i) {
    if (i > 0) {
      text += "   |   ";
    }

    text += QString("%1: %2").arg(kCategoryNames[i])
      .arg(composition.perCategory[i]);
  }

  return text;
}
