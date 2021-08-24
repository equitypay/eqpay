#include <qt/miningpage.h>
#include <qt/forms/ui_miningpage.h>

#include <qt/bitcoinunits.h>
#include <qt/clientmodel.h>
#include <qt/platformstyle.h>
#include <qt/transactionfilterproxy.h>
#include <qt/transactiontablemodel.h>
#include <qt/walletmodel.h>
#include <interfaces/wallet.h>
#include <qt/transactiondescdialog.h>
#include <qt/styleSheet.h>

#include <QSortFilterProxyModel>

Q_DECLARE_METATYPE(interfaces::WalletBalances)

#include <qt/miningpage.moc>

MiningPage::MiningPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage),
    clientModel(nullptr),
    walletModel(nullptr)
{
    ui->setupUi(this);
}

MiningPage::~MiningPage()
{
    delete ui;
}

void MiningPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
}

void MiningPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}

void MiningPage::setBalance(const interfaces::WalletBalances& balances)
{
}