#include "lfsr.h"
#include <algorithm>
#include <cmath>
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <string.h>
#include <vector>
#include "lfsrform.h"

Lfsr::Lfsr()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"polynomial", ParameterDelegate::ParameterType::String},
        {"taps", ParameterDelegate::ParameterType::String},
        {"bits_wanted", ParameterDelegate::ParameterType::Integer}
    };
    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
                        QString seed = parameters.value("polynomial").toString();
                        QString taps = parameters.value("taps").toString();
                        return QString("LFSR %1 %2").arg(seed).arg(taps);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new LfsrForm(delegate);
                });
}

ImporterExporterInterface *Lfsr::createDefaultImporterExporter()
{
    return new Lfsr();
}

/*
 * Returns name of plugin
 */
QString Lfsr::name()
{
    return "LFSR";
}

QString Lfsr::description()
{
    return "Generate bits using a linear-feedback shift register";
}

QStringList Lfsr::tags()
{
    return {"Generic"};
}

bool Lfsr::canExport()
{
    return false;
}

bool Lfsr::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> Lfsr::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> Lfsr::exportParameterDelegate()
{
    return nullptr;
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

/*
 * Returns the LFSR stream
 */
QSharedPointer<ImportResult> Lfsr::importBits(const Parameters &parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    int bitsWanted = parameters.value("bits_wanted").toInt();
    QString seed = parameters.value("polynomial").toString();
    QString lfsr = seed;
    QString usedTaps = parameters.value("taps").toString();

    // Displays warning if any input field is empty
    if (bitsWanted == 0 || seed == "" || usedTaps == "") {
        return ImportResult::error("Invalid Input. Please try again.");
    }

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(bitsWanted));

    std::vector<int> allTaps = findIntsSort(parameters.value("taps").toString());
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
            progress->setProgressPercent(nextPercent);
        }
        if (progress->isCancelled()) {
            return ImportResult::error("Processing cancelled");
        }

    } while (numBits != bitsWanted);

    return ImportResult::result(BitContainer::create(outputBits), parameters);
}

QSharedPointer<ExportResult> Lfsr::exportBits(QSharedPointer<const BitContainer> container, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(container)
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ExportResult::error("LFSR export is not supported.");
}
