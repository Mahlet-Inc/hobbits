#include "bitserror.h"
#include "cmath"
#include "ctime"
#include "ui_bitserror.h"
#include <QBitArray>
#include <QDebug>
#include <random>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

BitsError::BitsError() :
    ui(new Ui::BitsError())
{

}

QString BitsError::getName()
{
    return "Bit Error";
}

void BitsError::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
}

void BitsError::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

QSharedPointer<const OperatorResult> BitsError::getGaussianErrorBits(QSharedPointer<const BitContainer> input,
                                                  const QJsonObject &recallablePluginState,
                                                  QSharedPointer<ActionProgress> progressTracker) {
    qint64 bitLength = input->bits()->sizeInBits();

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(input->bits().data()));

    double coeff = (recallablePluginState.value("error_coeff")).toDouble();
    int exp = (recallablePluginState.value("error_exp")).toInt();

    double TrueBer = ((coeff * (pow(10, exp))));

    qint64 numBitsToFlip = qint64(floor(bitLength * TrueBer));
    qint64 incr = bitLength / numBitsToFlip;
    qint64 start = 0;
    qint64 end = bitLength / 2;
    qint64 counter = 0;
    qint64 mean = qint64(floor(end / 2));
    qint64 stddev = qint64(floor(end / 6)); // Empirical Rule

    std::normal_distribution<double> distribution(0, stddev);
    std::default_random_engine generator;

    while (counter != numBitsToFlip) {
        qint64 number = qint64(distribution(generator)) + mean;

        if (number < 0) {
            number += bitLength;
            if ((outputBits->at(number - 1) ^ 1) == 1) {
                outputBits->set(number - 1, true);
            }
            else {
                outputBits->set(number - 1, false);
            }
        }
        else if (number > bitLength) {
            number %= bitLength;
            if ((outputBits->at(number - 1) ^ 1) == 1) {
                outputBits->set(number - 1, true);
            }
            else {
                outputBits->set(number - 1, false);
            }
        }
        else if (number >= start && number <= end) {
            if ((outputBits->at(number - 1) ^ 1) == 1) {
                outputBits->set(number - 1, true);
            }
            else {
                outputBits->set(number - 1, false);
            }
        }

        mean += incr;
        start += incr;
        end += incr;
        counter++;

        progressTracker->setProgress(counter, numBitsToFlip);
        if (progressTracker->getCancelled()) {
            return OperatorResult::error("Process cancelled");
        }
    }

    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBits(outputBits);
    bitContainer->setName(QString("%1e%2 BER <- %3").arg(coeff).arg(exp).arg(input->name()));

    return OperatorResult::result({bitContainer}, recallablePluginState);
}

QSharedPointer<const OperatorResult> BitsError::getPeriodicErrorBits(QSharedPointer<const BitContainer> input,
                                                    const QJsonObject &recallablePluginState,
                                                    QSharedPointer<ActionProgress> progressTracker) {

    qint64 bitLength = input->bits()->sizeInBits();

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(input->bits().data()));

    double coeff = recallablePluginState.value("error_coeff").toDouble();
    int exp = int(recallablePluginState.value("error_exp").toDouble());

    double TrueBer = ((coeff * (pow(10, exp))));
    double ber = 1 / TrueBer;


    int skipStep = int(floor(ber));

    if (bitLength > 0) {
        for (qint64 i = skipStep - 1; i < bitLength; i += skipStep) {
            if ((outputBits->at(i) ^ 1) == 0) {
                outputBits->set(i, false);
            }
            else {
                outputBits->set(i, true);
            }

            progressTracker->setProgress(i, bitLength);
            if (progressTracker->getCancelled()) {
                return OperatorResult::error("Process cancelled");
            }
        }
    }

    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBits(outputBits);
    bitContainer->setName(QString("%1e%2 BER <- %3").arg(coeff).arg(exp).arg(input->name()));

    return OperatorResult::result({ bitContainer }, recallablePluginState);
}

QJsonObject BitsError::getStateFromUi()
{
    QJsonObject pluginState;

    pluginState.insert("error_coeff", (ui->coeffInput->text()).toDouble());
    pluginState.insert("error_exp", (ui->expInput->value()));
    if (ui->gaussianOpt->isChecked()) {
        pluginState.insert("error_type", "gaussian");
    }
    else {
        pluginState.insert("error_type", "periodic");
    }
    return pluginState;
}

bool BitsError::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->coeffInput->setText(QString("%1").arg(pluginState.value("error_coeff").toDouble()));
    ui->expInput->setValue(pluginState.value("error_exp").toInt());
    if (pluginState.value("error_type").toString() == "periodic") {
        ui->periodicOpt->click();
    }
    else {
        ui->gaussianOpt->click();
    }
    return true;
}

bool BitsError::canRecallPluginState(const QJsonObject &pluginState)
{
    if (pluginState.contains("error_coeff") && pluginState.value("error_coeff").isDouble()) {
        if (pluginState.contains("error_exp")) {
            if ((pluginState.contains("error_type") && pluginState.value("error_type").isString())) {
                return true;
            }
        }
    }
    return false;
}

int BitsError::getMinInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

int BitsError::getMaxInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

QSharedPointer<const OperatorResult> BitsError::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    if (inputContainers.size() != 1) {
        return OperatorResult::error("Requires a single bit container as input");
    }

    if (recallablePluginState.value("error_type").toString() == "gaussian") {
        return getGaussianErrorBits(inputContainers.at(0), recallablePluginState, progressTracker);
    }
    else {
        return getPeriodicErrorBits(inputContainers.at(0), recallablePluginState, progressTracker);
    }
}

OperatorInterface* BitsError::createDefaultOperator()
{
    return new BitsError();
}

void BitsError::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
}
