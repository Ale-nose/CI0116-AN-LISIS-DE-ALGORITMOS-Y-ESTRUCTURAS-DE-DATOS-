// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "ChallengeLeaderboardDialog.hpp"
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <vector>

ChallengeLeaderboardDialog::ChallengeLeaderboardDialog(
    const std::vector<ChallengeResult>& results,
    QWidget* parent)
    : QDialog(parent) {

  setWindowTitle("Challenge Leaderboard");
  setMinimumSize(520, 360);

  auto* layout = new QVBoxLayout(this);

  auto* table = new QTableWidget(static_cast<int>(results.size()), 4, this);

  table->setHorizontalHeaderLabels({"Player", "Waves", "Lives", "Credits"});

  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);

  table->verticalHeader()->setVisible(false);
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  for (std::size_t i = 0; i < results.size(); ++i) {
    const ChallengeResult& result = results[i];
    const int row = static_cast<int>(i);

    table->setItem(
        row, 0,
        new QTableWidgetItem(QString::fromStdString(result.player)));

    table->setItem(
        row, 1,
        new QTableWidgetItem(QString::number(result.wavesCompleted)));

    table->setItem(
        row, 2,
        new QTableWidgetItem(QString::number(result.livesRemaining)));

    table->setItem(
        row, 3,
        new QTableWidgetItem(QString::number(result.creditsRemaining)));
  }

  auto* closeButton = new QPushButton("Close", this);

  connect(
      closeButton,
      &QPushButton::clicked,
      this,
      &QDialog::accept);

  layout->addWidget(table);
  layout->addWidget(closeButton);

  setStyleSheet(
    "QDialog {"
    "background-color: #172019;"
    "}"
    "QTableWidget {"
    "background-color: #223126;"
    "color: #e8eee6;"
    "gridline-color: #526b56;"
    "border: 2px solid #526b56;"
    "}"
    "QHeaderView::section {"
    "background-color: #1b2a1d;"
    "color: #d8e5d5;"
    "padding: 7px;"
    "border: 1px solid #526b56;"
    "font-weight: bold;"
    "}"
    "QPushButton {"
    "background-color: #3d5740;"
    "color: white;"
    "border: 1px solid #74906f;"
    "border-radius: 6px;"
    "padding: 8px;"
    "font-weight: bold;"
    "}"
    "QPushButton:hover {"
    "background-color: #526f56;"
    "}");
}
