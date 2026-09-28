// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <QDialog>
#include <vector>

#include "ChallengeMode.hpp"

class ChallengeLeaderboardDialog : public QDialog {
 public:
  explicit ChallengeLeaderboardDialog(
      const std::vector<ChallengeResult>& results,
      QWidget* parent = nullptr);
};
