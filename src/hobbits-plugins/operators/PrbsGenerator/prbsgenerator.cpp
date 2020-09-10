#include "prbsgenerator.h"
#include "ui_prbsgenerator.h"
#include <algorithm>
#include <cmath>
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <string.h>
#include <vector>

PrbsGenerator::PrbsGenerator() :
    ui(new Ui::PrbsGenerator()),
    m_stateHelper(new PluginStateHelper())
{
    m_stateHelper->addLineEditStringParameter("polynomial", [this](){return ui->initseedInput;});
    m_stateHelper->addLineEditStringParameter("taps", [this](){return ui->tapInput;});

    m_stateHelper->addParameter("bits_wanted", QJsonValue::Double, [this](QJsonValue value) {
        ui->bitsWantedInput->setText(QString("%1").arg(value.toInt()));
        return true;
    }, [this]() {
        MathParser mp;
        ParseResult a = mp.parseInput(ui->bitsWantedInput->text());
        if (a.isValid()) {
            return QJsonValue(a.getResult());
        }
        else {
            return QJsonValue();
        }
    });
}

/*
 * Returns name of plugin
 */
QString PrbsGenerator::getName()
{
    return "LFSR";
}

void PrbsGenerator::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
    connect(ui->btnTaps, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void PrbsGenerator::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

/* Outputs the ending state of the LFSR after
 * howevery many bits are outputted
 */
void PrbsGenerator::updateEndState(QString lfsr)
{
    ui->endStateOutput->setText(lfsr);
}

/*
 * Returns the integers in a String
 * sorted from greatest to least
 */
std::vector<int> findIntsSort(QString exp)
{

    std::vector<int> taps;

    for (int i = 0; i < exp.length(); i++) {
        if (exp[i].isDigit()) {
            QString val = "";
            while (i < exp.length() && exp[i].isDigit()) {
                val += exp[i];
                i++;
            }
            int tap = val.toInt();
            taps.push_back(tap);

        }
    }
    std::sort(taps.begin(), taps.end(), std::greater<int>());

    return taps;
}

/* Returns the integers in a String
 * with no sorting
 */
std::vector<int> findIntsNoSort(QString exp)
{

    std::vector<int> nums;


    for (int i = 0; i < exp.length(); i++) {
        if (exp[i].isDigit()) {
            QString val = "";
            while (i < exp.length() && exp[i].isDigit()) {
                val += exp[i];
                i++;
            }
            int tap = val.toInt();
            nums.push_back(tap);
        }
    }

    return nums;
}

/*
 * Returns all the operations in a string
 * (+, -, *, /)
 */
std::vector<QChar> findOps(QString exp)
{
    std::vector<QChar> ops;

    for (QChar x : exp) {
        if (x.isDigit() == false) {
            ops.push_back(x);
        }
    }

    return ops;
}

/*
 * Performs given operation on two integers
 */
int applyOp(int a, int b, QChar op)
{
    if (op == '*') {
        return a * b;
    }
    else if (op == '/') {
        return a / b;
    }
    else if (op == '+') {
        return a + b;
    }
    else if (op == '-') {
        return a - b;
    }

    return 0;
}

QJsonObject PrbsGenerator::getStateFromUi()
{
    return m_stateHelper->getPluginStateFromUi();
}

bool PrbsGenerator::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

bool PrbsGenerator::canRecallPluginState(const QJsonObject &pluginState)
{
    return m_stateHelper->validatePluginState(pluginState);
}

int PrbsGenerator::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 0;
}

int PrbsGenerator::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 0;
}

/*
 * Returns the PRBS stream
 */
QSharedPointer<const OperatorResult> PrbsGenerator::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    Q_UNUSED(inputContainers)
    QList<QSharedPointer<BitContainer>> outputContainers;

    int bitsWanted = recallablePluginState.value("bits_wanted").toInt();
    QString seed = recallablePluginState.value("polynomial").toString();
    QString lfsr = seed;
    QString usedTaps = recallablePluginState.value("taps").toString();

    // Displays warning if any input field is empty
    if (bitsWanted == 0 || seed == "" || usedTaps == "") {
        return OperatorResult::error("Invalid Input. Please try again.");
    }

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(bitsWanted));

    std::vector<int> allTaps = findIntsSort(recallablePluginState.value("taps").toString());
    qint64 numBits = 0;
    qint64 outputIndex = 0;
    int lastPercent = 0;

    do {
        int bitToAdd = 0;
        for (uint i = 0; i < allTaps.size(); i++) {

            int tapIndex = seed.length() - allTaps[i];

            QStringRef _bitAtIndex(&lfsr, tapIndex, 1);
            int bitAtIndex = _bitAtIndex.toInt();

            bitToAdd ^= bitAtIndex;

        }

        QStringRef _bitForPRBS(&lfsr, 0, 1);
        int bitForPRBS = _bitForPRBS.toInt();
        outputBits->set(outputIndex, bitForPRBS);

        QString appendBit = QString::number(bitToAdd);
        lfsr += appendBit;
        //
        lfsr.remove(0, 1);

        ++outputIndex;
        ++numBits;

        int nextPercent = int(double(numBits) / double(bitsWanted) * 100.0);
        if (nextPercent > lastPercent) {
            lastPercent = nextPercent;
            progressTracker->setProgressPercent(nextPercent);
        }
        if (progressTracker->getCancelled()) {
            return QSharedPointer<const OperatorResult>(
                    (new OperatorResult())->setPluginState(
                            QJsonObject(
                                    {QPair<QString, QJsonValue>(
                                            "error",
                                            QJsonValue("Processing cancelled"))}))
                    );
        }

    } while (numBits != bitsWanted);


    updateEndState(lfsr);
    auto bitContainer = BitContainer::create(outputBits);
    outputContainers.append(bitContainer);

    return OperatorResult::result(outputContainers, recallablePluginState);
}

void PrbsGenerator::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
}

OperatorInterface* PrbsGenerator::createDefaultOperator()
{
    return new PrbsGenerator();
}

/*
 * Displays message box indicating
 * how to proeprly input which taps you're using
 */
void PrbsGenerator::showHelp()
{
    QMessageBox msg;
    msg.setText("Important Note:");
    msg.setInformativeText("Separate each tap by a ','");
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();

}
