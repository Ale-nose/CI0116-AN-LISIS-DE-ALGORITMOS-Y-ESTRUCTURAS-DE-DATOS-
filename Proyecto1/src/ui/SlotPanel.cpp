#include "SlotPanel.hpp"

#include <QVBoxLayout>

SlotPanel::SlotPanel(QWidget* parent) : QFrame(parent) {
  setMinimumSize(150, 100);
  setFrameStyle(QFrame::Box | QFrame::Plain);

  // Create vertical layout manager to stack widgets top-to-bottom
  auto* layout = new QVBoxLayout(this);

  structureLabel_ = new QLabel("Empty", this);
  sizeLabel_ = new QLabel("Enemies tracked: 0", this);
  lagBar_ = new QProgressBar(this);
  lagBar_->setRange(0, 100);
  lagBar_->setValue(0);

  layout->addWidget(structureLabel_);
  layout->addWidget(sizeLabel_);
  layout->addWidget(lagBar_);

  setLayout(layout);
}

void SlotPanel::showEmpty() {
  structureLabel_->setText("Empty");
  sizeLabel_->setText("Enemies tracked: 0");
  lagBar_->setValue(0);
}

void SlotPanel::showOccupied(CoreType type, std::size_t enemiesTracked, int lagPercent) {
  structureLabel_->setText(coreTypeName(type));
  sizeLabel_->setText(QString("Enemies tracked: %1").arg(enemiesTracked));
  lagBar_->setValue(lagPercent);
}

QString SlotPanel::coreTypeName(CoreType type) {
  // Map core type enumeration value to printable UI string
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
