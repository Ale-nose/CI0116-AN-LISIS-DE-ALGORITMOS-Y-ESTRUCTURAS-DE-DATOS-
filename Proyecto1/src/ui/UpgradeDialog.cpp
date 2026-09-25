#include "UpgradeDialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include "CoreDisplay.hpp"

namespace {
constexpr int kCoreTypeRole = Qt::UserRole;
}  // namespace

UpgradeDialog::UpgradeDialog(int slotIndex, const Economy& economy, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Ranura %1 — elegir estructura").arg(slotIndex));

    auto* layout = new QVBoxLayout(this);

    auto* creditsLabel = new QLabel(
        QStringLiteral("Créditos disponibles: %1").arg(economy.getCredits()), this);
    layout->addWidget(creditsLabel);

    list_ = new QListWidget(this);
    for (CoreType type : allCoreTypes())
    {
        int price = corePrice(type);
        auto* item = new QListWidgetItem(
            QStringLiteral("%1 — %2 créditos").arg(coreDisplayName(type)).arg(price));
        item->setData(kCoreTypeRole, static_cast<int>(type));

        if (!economy.canAfford(price))
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }

        list_->addItem(item);
    }
    layout->addWidget(list_);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

std::optional<CoreType> UpgradeDialog::selectedCore() const
{
    QListWidgetItem* item = list_->currentItem();
    if (!item)
    {
        return std::nullopt;
    }
    return static_cast<CoreType>(item->data(kCoreTypeRole).toInt());
}