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
#include <qt/transactionview.h>
#include <qt/styleSheet.h>
#include <qt/bitcoinunits.h>

#include <util/check.h>
#include <validation.h>
#include <miner.h>
#include <pow.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QTimer>

Q_DECLARE_METATYPE(interfaces::WalletBalances)

#include <qt/miningpage.moc>

QString FormatHashrate(double hashrate) {
    uint64_t nHashrate = hashrate;

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

double GetDifficulty(uint32_t nBits)
{
    int nShift = (nBits >> 24) & 0xff;
    double dDiff =
        (double)0x0000ffff / (double)(nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

double GetNetworkHashPM() {
    return ((GetDifficulty(GetNextWorkRequired(::ChainActive().Tip(), Params().GetConsensus(), false)) * pow(2, 32) * 10 / 60)) * 60;
}

MiningPage::MiningPage(const PlatformStyle *_platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage),
    platformStyle(_platformStyle),
    clientModel(nullptr),
    walletModel(nullptr),
    miningState(false),
    transactionView(0)
{
    ui->setupUi(this);
    transactionView = new TransactionView(platformStyle, this, true);
    ui->frameMiningRecords->layout()->addWidget(transactionView);

    ui->threadSlider->setMinimum(1);
    ui->threadSlider->setMaximum(std::thread::hardware_concurrency() / 2);
    ui->threadSlider->setTickPosition(QSlider::TicksBothSides);
    ui->threadSlider->setTickInterval(1);
    ui->threadSlider->setSingleStep(1);

    ui->threadLabel->setText(QString::number(1));
    ui->networkLabel->setText(FormatHashrate(GetNetworkHashPM()));

    updateMiningStatsTimer = new QTimer(this);
    updateNethashTimer = new QTimer(this);

    connect(updateMiningStatsTimer, &QTimer::timeout, this, &MiningPage::updateMiningStatistics);
    connect(updateNethashTimer, &QTimer::timeout, this, &MiningPage::updateNetworkStatistics);
    connect(ui->threadSlider, &QSlider::valueChanged, this, &MiningPage::updateThreads);

    updateNethashTimer->start(1000);
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
    if(model && model->getOptionsModel())
    {
        transactionView->setModel(model);
        transactionView->chooseType(6);
    }
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

        ui->labelDaily->setText("0.00000000 EQPAY");
        ui->labelMonthly->setText("0.00000000 EQPAY");
        ui->labelYearly->setText("0.00000000 EQPAY");
    }
}

void MiningPage::updateMiningStatistics()
{
    ui->hashrateLabel->setText(FormatHashrate(hashrate));

    CAmount nSubsidy = GetBlockSubsidy(::ChainActive().Tip()->nHeight, Params().GetConsensus());

    CAmount nDailyPoW = (nSubsidy * 1440) / 2;
    CAmount nMonthlyPoW = (nSubsidy * 43830) / 2;
    CAmount nYearlyPoW = (nSubsidy * 525960) / 2;

    double nethash = GetNetworkHashPM();
    double percentage = hashrate / nethash;

    if (percentage > 1)
        percentage = 1;

    ui->labelDaily->setText(BitcoinUnits::format(0, percentage * nDailyPoW) + " EQPAY");
    ui->labelMonthly->setText(BitcoinUnits::format(0, percentage * nMonthlyPoW) + " EQPAY");
    ui->labelYearly->setText(BitcoinUnits::format(0, percentage * nYearlyPoW) + " EQPAY");
}

void MiningPage::updateNetworkStatistics()
{
    ui->networkLabel->setText(FormatHashrate(GetNetworkHashPM()));
}

void MiningPage::updateThreads(int value)
{
    ui->threadLabel->setText(QString::number(value));
    manageMiningState(miningState, value);
}
