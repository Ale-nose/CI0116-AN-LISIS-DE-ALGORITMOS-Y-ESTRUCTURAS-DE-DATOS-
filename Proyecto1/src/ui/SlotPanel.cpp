// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "SlotPanel.hpp"
#include <QMouseEvent>
#include <QVBoxLayout>

SlotPanel::SlotPanel(QWidget* parent) : QFrame(parent) {
  setObjectName("slotPanel");
  setMinimumSize(150, 96);

  structureLabel_ = new QLabel("EMPTY", this);
  sizeLabel_ = new QLabel("Enemies tracked: 0", this);
  lagBar_ = new QProgressBar(this);

  structureLabel_->setObjectName("structureLabel");
  sizeLabel_->setObjectName("sizeLabel");
  lagBar_->setObjectName("lagBar");

  structureLabel_->setAlignment(Qt::AlignCenter);
  sizeLabel_->setAlignment(Qt::AlignCenter);

  lagBar_->setRange(0, 100);
  lagBar_->setValue(0);
  lagBar_->setTextVisible(true);
  lagBar_->setFormat("Lag %p%");

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 8, 10, 8);
  layout->setSpacing(6);
  layout->addWidget(structureLabel_);
  layout->addWidget(sizeLabel_);
  layout->addWidget(lagBar_);

  setStyleSheet(
    "#slotPanel {"
    "background-color: #223126;"
    "border: 2px solid #526b56;"
    "border-radius: 9px;"
    "}"
    "#slotPanel:hover {"
    "background-color: #2b3e30;"
    "border: 2px solid #82a17e;"
    "}"
    "#structureLabel {"
    "color: #e8eee6;"
    "font-size: 13px;"
    "font-weight: bold;"
    "}"
    "#sizeLabel {"
    "color: #b8c7b6;"
    "font-size: 11px;"
    "}"
    "#lagBar {"
    "background-color: #121914;"
    "color: white;"
    "border: 1px solid #455849;"
    "border-radius: 5px;"
    "min-height: 16px;"
    "text-align: center;"
    "}"
    "#lagBar::chunk {"
    "background-color: #d0a142;"
    "border-radius: 4px;"
    "}");
}

void SlotPanel::mousePressEvent(QMouseEvent* /*event*/) {
  Q_EMIT clicked();
}

void SlotPanel::showEmpty() {
  structureLabel_->setText("EMPTY");
  sizeLabel_->setText("No core installed");
  lagBar_->setValue(0);
}

void SlotPanel::showOccupied(
  CoreType type, std::size_t enemiesTracked, int lagPercent) {
  structureLabel_->setText(coreTypeName(type));
  sizeLabel_->setText(
    QString("Enemies tracked: %1").arg(enemiesTracked));
  lagBar_->setValue(lagPercent);
}

QString SlotPanel::coreTypeName(CoreType type) {
  switch (type) {
    case CoreType::LinkedList:
      return "Linked List";
    case CoreType::SortedList:
      return "Sorted List";
    case CoreType::DynamicArray:
      return "Dynamic Array";
    case CoreType::SortedArray:
      return "Sorted Array";
    case CoreType::Bst:
      return "BST";
    case CoreType::Avl:
      return "AVL";
    case CoreType::MinHeap:
      return "Min Heap";
    case CoreType::HashTable:
      return "Hash Table";
  }

  return "Unknown";
}
