// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez

#include "MapView.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>

#include <algorithm>

#include "CombatConstants.hpp"
#include "MapBuilder.hpp"

MapView::MapView(QWidget* parent) : QWidget(parent) {
  buildDefaultMap(grid_);
  setFixedSize(GRID_WIDTH * kCellSize, GRID_HEIGHT * kCellSize);

  const QString assetsPath = QDir::cleanPath(
      QCoreApplication::applicationDirPath() + "/../../assets");

  mapPixmap_.load(assetsPath + "/Map.png");

  towerSprites_[static_cast<std::size_t>(CoreType::LinkedList)]
      .load(assetsPath + "/towers/ll.png");

  towerSprites_[static_cast<std::size_t>(CoreType::SortedList)]
      .load(assetsPath + "/towers/sl.png");

  towerSprites_[static_cast<std::size_t>(CoreType::DynamicArray)]
      .load(assetsPath + "/towers/da.png");

  towerSprites_[static_cast<std::size_t>(CoreType::SortedArray)]
      .load(assetsPath + "/towers/sa.png");

  towerSprites_[static_cast<std::size_t>(CoreType::Bst)]
      .load(assetsPath + "/towers/bst.png");

  towerSprites_[static_cast<std::size_t>(CoreType::Avl)]
      .load(assetsPath + "/towers/avl.png");

  towerSprites_[static_cast<std::size_t>(CoreType::MinHeap)]
      .load(assetsPath + "/towers/hp.png");

  towerSprites_[static_cast<std::size_t>(CoreType::HashTable)]
      .load(assetsPath + "/towers/ht.png");

  enemySprites_[static_cast<std::size_t>(EnemyCategory::Swarm)]
      .load(assetsPath + "/enemies/swarm.png");

  enemySprites_[static_cast<std::size_t>(EnemyCategory::Wraith)]
      .load(assetsPath + "/enemies/wraith.png");

  enemySprites_[static_cast<std::size_t>(EnemyCategory::Hive)]
      .load(assetsPath + "/enemies/hive.png");

  enemySprites_[static_cast<std::size_t>(EnemyCategory::Decoy)]
      .load(assetsPath + "/enemies/decoy.png");

  enemySprites_[static_cast<std::size_t>(EnemyCategory::Colossus)]
      .load(assetsPath + "/enemies/colossus.png");
}

void MapView::refresh(const WorldState& state, const SlotManager& slotManager) {
  worldState_ = state;

  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    installedCores_[i] = slotManager.installedType(i);
  }
  update();
}

void MapView::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  // DRAW MAP
  if (!mapPixmap_.isNull()) {
    painter.drawPixmap(QRect(0, 0, GRID_WIDTH * kCellSize,
      GRID_HEIGHT * kCellSize), mapPixmap_);
  }

  // DRAW TOWERS
  const auto& positions = towerSlotPositions();

  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    if (!installedCores_[i]) {
      continue;
    }

    CoreType type = *installedCores_[i];

    const QPixmap& sprite = towerSprites_[static_cast<std::size_t>(type)];

    if (sprite.isNull()) {
      continue;
    }

    const int gridX = positions[i].first;
    const int gridY = positions[i].second;

    QRect cellRect(
      gridX * kCellSize,
      gridY * kCellSize,
      kCellSize,
      kCellSize);

    QRect spriteRect = cellRect.adjusted(1, 1, -1, -1);

    painter.drawPixmap(spriteRect, sprite);
  }

  // DRAW ENEMIES
  for (const auto& enemy : worldState_.enemies) {
    const QPixmap& sprite = enemySprites_[static_cast<std::size_t>(
      enemy.category)];

    if (sprite.isNull()) {
      continue;
    }

    int spriteSize = 22;

    if (enemy.category == EnemyCategory::Colossus) {
      spriteSize = 28;
    }

    // Small offset so enemies sharing a
    // cell are not completely identical
    // on top of each other.
    int offsetX = static_cast<int>(enemy.id % 3) - 1;
    int offsetY = static_cast<int>((enemy.id / 3) % 3) - 1;

    offsetX *= 2;
    offsetY *= 2;

    const int centerX = enemy.gridX * kCellSize + kCellSize / 2 + offsetX;

    const int centerY = enemy.gridY * kCellSize + kCellSize / 2 + offsetY;

    QRect spriteRect(
      centerX - spriteSize / 2,
      centerY - spriteSize / 2,
      spriteSize,
      spriteSize);

    painter.drawPixmap(spriteRect, sprite);
  }

  // DRAW HOLLOW PURPLE CHANT (on top of everything)
  if (worldState_.hollow_purple_charge_left > 0) {
    drawHollowPurpleChant(painter);
  }
}

void MapView::drawHollowPurpleChant(QPainter& painter) {
  static const std::array<QString, 4> kChant = {
    QStringLiteral("〝九綱〟"),
    QStringLiteral("〝偏光〟"),
    QStringLiteral("〝烏と声明〟"),
    QStringLiteral("〝表裏の間〟"),
  };
  const int lineCount = static_cast<int>(kChant.size());

  // Line k appears once k/lineCount of the charge has elapsed.
  const int elapsed =
    HOLLOW_PURPLE_CHARGE_TICKS - worldState_.hollow_purple_charge_left;
  const int shown = std::min(lineCount,
    elapsed * lineCount / HOLLOW_PURPLE_CHARGE_TICKS + 1);

  painter.fillRect(rect(), QColor(40, 0, 70, kChantVeilAlpha));

  QFont font = painter.font();
  font.setPointSize(kChantFontPointSize);
  font.setBold(true);
  painter.setFont(font);
  painter.setPen(QColor(215, 175, 255));

  const int lineHeight = QFontMetrics(font).height();
  const int top = rect().center().y() - shown * lineHeight / 2;
  for (int k = 0; k < shown; ++k) {
    painter.drawText(QRect(0, top + k * lineHeight, width(), lineHeight),
      Qt::AlignCenter, kChant[static_cast<std::size_t>(k)]);
  }
}

void MapView::mousePressEvent(QMouseEvent* event) {
  int cellX = event->pos().x() / kCellSize;
  int cellY = event->pos().y() / kCellSize;

  if (!Grid::inBounds(cellX, cellY)) {
    return;
  }

  if (grid_.at(cellX, cellY) != CellType::TowerSlot) {
    return;
  }

  const auto& positions = towerSlotPositions();

  for (std::size_t i = 0; i < positions.size(); ++i) {
    if (positions[i].first == cellX && positions[i].second == cellY) {
      Q_EMIT slotClicked(static_cast<int>(i));
      return;
    }
  }
}
