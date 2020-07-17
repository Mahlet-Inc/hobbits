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
    ui(new Ui::BitsError()),
    m_stateHelper(new PluginStateHelper())
{
    m_stateHelper->addParameter("error_coeff", QJsonValue::Double, [this](QJsonValue value) {
        ui->coeffInput->setText(QString("%1").arg(value.toDouble()));
        return true;
    }, [this]() {
        return QJsonValue(ui->coeffInput->text().toDouble());
    });

    m_stateHelper->addSpinBoxIntParameter("error_exp", [this](){return ui->expInput;});

    m_stateHelper->addParameter("error_type", QJsonValue::String, [this](QJsonValue value) {
        if (value.toString() == "periodic") {
            ui->periodicOpt->setChecked(true);
        }
        else {
            ui->gaussianOpt->setChecked(true);
        }
        return true;
    }, [this]() {
        if (ui->periodicOpt->isChecked()) {
            return QJsonValue("periodic");
        }
        else {
            return QJsonValue("gaussian");
        }
    });
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
                                                                     double ber,
                                                                     const QJsonObject &recallablePluginState,
                                                  QSharedPointer<ActionProgress> progressTracker) {
    qint64 bitLength = input->bits()->sizeInBits();

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(input->bits().data()));

    qint64 numBitsToFlip = qint64(floor(bitLength * ber));
    if (numBitsToFlip > 0) {
        qint64 incr = bitLength / numBitsToFlip;
        qint64 mean = 0;
        qint64 distributionRange = incr * 3;
        double stddev = double(distributionRange) / 6.0; // Empirical Rule

        auto dist = std::bind(std::normal_distribution<double>{0, stddev},
                              std::default_random_engine(std::random_device{}()));

        for (qint64 i = 0; i < numBitsToFlip; i++) {
            qint64 number = qint64(round(dist())) + mean;
            while (number < 0) {
                number += bitLength;
            }
            qint64 idx = number % bitLength;

            if (outputBits->at(idx)) {
                outputBits->set(idx, false);
            }
            else {
                outputBits->set(idx, true);
            }

            mean += incr;

            progressTracker->setProgress(i, numBitsToFlip);
            if (progressTracker->getCancelled()) {
                return OperatorResult::error("Process cancelled");
            }
        }
    }
    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBits(outputBits);
    bitContainer->setName(QString("%1 BER <- %2").arg(ber, 0, 'e', 2).arg(input->name()));

    return OperatorResult::result({bitContainer}, recallablePluginState);
}

QSharedPointer<const OperatorResult> BitsError::getPeriodicErrorBits(QSharedPointer<const BitContainer> input,
                                                                     double ber,
                                                                     const QJsonObject &recallablePluginState,
                                                    QSharedPointer<ActionProgress> progressTracker) {
    qint64 bitLength = input->bits()->sizeInBits();
    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(input->bits().data()));

    double errorPeriod = 1.0 / ber;

    if (bitLength > 0) {
        for (double i = errorPeriod; qint64(round(i)) < bitLength; i += errorPeriod) {
            qint64 idx = qint64(round(i));
            if (outputBits->at(idx)) {
                outputBits->set(idx, false);
            }
            else {
                outputBits->set(idx, true);
            }

            progressTracker->setProgress(idx, bitLength);
            if (progressTracker->getCancelled()) {
                return OperatorResult::error("Process cancelled");
            }
        }
    }

    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBits(outputBits);
    bitContainer->setName(QString("%1 BER <- %2").arg(ber, 0, 'e', 2).arg(input->name()));

    return OperatorResult::result({ bitContainer }, recallablePluginState);
}

QJsonObject BitsError::getStateFromUi()
{
    return m_stateHelper->getPluginStateFromUi();
}

bool BitsError::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

bool BitsError::canRecallPluginState(const QJsonObject &pluginState)
{
    return m_stateHelper->validatePluginState(pluginState);
}

int BitsError::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int BitsError::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
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

    double coeff = recallablePluginState.value("error_coeff").toDouble();
    double exp = recallablePluginState.value("error_exp").toDouble();
    double ber = ((coeff * (pow(10, exp))));

    if (ber > 1.0) {
        return OperatorResult::error("Cannot have an error rate exceeding 100%");
    }
    if (ber <= 0.0) {
        return OperatorResult::error("Cannot have an error rate of 0% or less");
    }


    if (recallablePluginState.value("error_type").toString() == "gaussian") {
        return getGaussianErrorBits(inputContainers.at(0), ber, recallablePluginState, progressTracker);
    }
    else {
        return getPeriodicErrorBits(inputContainers.at(0), ber, recallablePluginState, progressTracker);
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
