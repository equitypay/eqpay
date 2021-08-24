#ifndef QTUM_QT_STAKEPAGE_H
#define QTUM_QT_STAKEPAGE_H

#include <interfaces/wallet.h>

#include <QWidget>
#include <memory>

class ClientModel;
class TransactionFilterProxy;
class PlatformStyle;
class WalletModel;

namespace Ui {
    class MiningPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Mining page widget */
class MiningPage : public QWidget
{
    Q_OBJECT

public:
    explicit MiningPage(const PlatformStyle *platformStyle, QWidget *parent = nullptr);
    ~MiningPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);

public Q_SLOTS:
    void setBalance(const interfaces::WalletBalances& balances);

Q_SIGNALS:
    void transactionClicked(const QModelIndex &index);

private:
    Ui::MiningPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;

private Q_SLOTS:
};

#endif // QTUM_QT_STAKEPAGE_H