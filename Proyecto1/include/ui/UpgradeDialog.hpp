#pragma once

#include <optional>
#include <QDialog>
#include "CorePrices.hpp"
#include "Economy.hpp"

class QListWidget;

// Section 5.3 — the single dialog that resolves the construction phase:
// the player picks a slot (already chosen by the caller), and this
// dialog shows every available structure with its price, greying out
// what the player can't afford. No free placement, no selling, no
// repositioning — the player only ever picks a structure to install in
// the slot they clicked.
class UpgradeDialog : public QDialog {
    Q_OBJECT

public:
    // slotIndex is shown in the title only, for the player's context.
    // economy is read-only here: this dialog never deducts credits
    // itself — the caller does that (economy.buyCore(type)) only after
    // the player confirms, then installs the core via SlotManager.
    UpgradeDialog(int slotIndex, const Economy& economy, QWidget* parent = nullptr);

    // Valid only after exec() returns QDialog::Accepted.
    std::optional<CoreType> selectedCore() const;

private:
    QListWidget* list_ = nullptr;
};