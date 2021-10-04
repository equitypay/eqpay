#ifndef QTUM_QT_STAKEPAGE_H
#define QTUM_QT_STAKEPAGE_H

#include <qt/transactionview.h>
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

    bool miningState;
    void manageMiningState(bool state, int nThreads);
    void updateMiningStatistics();
    void updateNetworkStatistics();
    void updateThreads(int value);

private:
    Ui::MiningPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;

    QTimer *updateMiningStatsTimer;
    QTimer *updateNethashTimer;

    const PlatformStyle* const platformStyle;
    TransactionView* transactionView;

private Q_SLOTS:
    void on_checkEnableAdvanced_clicked(bool checked);
    void on_miningButton_clicked();
};

#endif // QTUM_QT_STAKEPAGE_H