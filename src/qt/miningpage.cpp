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

#include <validation.h>
#include <miner.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QTimer>

Q_DECLARE_METATYPE(interfaces::WalletBalances)

#include <qt/miningpage.moc>

QString FormatHashrate(double hashrate) {
    int nHashrate = hashrate;

    if (nHashrate >= 1000000000) {
        return QString::number(nHashrate / 1000000000) + " Gh/m";
    } else if (nHashrate >= 1000000) {
        return QString::number(nHashrate / 1000000) + " Mh/m";
    } else if (nHashrate >= 1000) {
        return QString::number(nHashrate / 1000) + " Kh/m";
    } else {
        return QString::number(nHashrate) + " H/m";
    }
}

MiningPage::MiningPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage),
    clientModel(nullptr),
    walletModel(nullptr),
    miningState(false)
{
    ui->setupUi(this);

    ui->threadSlider->setMinimum(1);
    ui->threadSlider->setMaximum(std::thread::hardware_concurrency());
    ui->threadSlider->setTickPosition(QSlider::TicksBothSides);
    ui->threadSlider->setTickInterval(1);
    ui->threadSlider->setSingleStep(1);

    ui->threadLabel->setText(QString::number(1));

    updateMiningStatsTimer = new QTimer(this);
    connect(updateMiningStatsTimer, &QTimer::timeout, this, &MiningPage::updateMiningStatistics);
    connect(ui->threadSlider, &QSlider::valueChanged, this, &MiningPage::updateThreads);
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

void MiningPage::manageMiningState(bool state, int nThreads)
{
    if (!walletModel)
        return;

    if (state != miningState)
        miningState = state;

    if (!miningState) {
        updateMiningStatsTimer->stop();
        walletModel->wallet().stopMining();  
        ui->miningButton->setText("Start mining");
    } else {
        // Update stats every 5s
        updateMiningStatsTimer->start(1000);
        walletModel->wallet().startMining(nThreads);
        ui->miningButton->setText("Stop mining");
    }
}

void MiningPage::on_miningButton_clicked()
{
    if (!miningState) {
        int nThreads = ui->threadSlider->value();
        MiningPage::manageMiningState(true, nThreads);
    } else {
        MiningPage::manageMiningState(false, 0);
        ui->hashrateLabel->setText("0 H/m");
    }
}

void MiningPage::updateMiningStatistics()
{
    ui->hashrateLabel->setText(FormatHashrate(hashrate));
}

void MiningPage::updateThreads(int value)
{
    ui->threadLabel->setText(QString::number(value));
    manageMiningState(miningState, value);
}
