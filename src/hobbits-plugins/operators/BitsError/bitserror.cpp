#include "bitserror.h"
#include "cmath"
#include "ctime"
#include <QBitArray>
#include <QDebug>
#include <random>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bitserrorform.h"

BitsError::BitsError()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"error_coeff", QJsonValue::Double},
        {"error_exp", QJsonValue::Double},
        {"error_type", QJsonValue::String}
    };

    m_delegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    infos,
                    [](const QJsonObject &parameters) {
                        double coeff = parameters.value("error_coeff").toDouble();
                        double exp = parameters.value("error_exp").toDouble();
                        double ber = ((coeff * (pow(10, exp))));

                        return QString("%1 BER").arg(ber, 0, 'e', 2);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new BitsErrorForm(delegate);
                    }));
}

OperatorInterface* BitsError::createDefaultOperator()
{
    return new BitsError();
}

QString BitsError::name()
{
    return "Bit Error";
}

QString BitsError::description()
{
    return "Add Bit Errors";
}

QStringList BitsError::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> BitsError::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const OperatorResult> BitsError::getGaussianErrorBits(QSharedPointer<const BitContainer> input,
                                                                     double ber,
                                                                     const QJsonObject &recallablePluginState,
                                                  QSharedPointer<PluginActionProgress> progressTracker) {
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
            if (progressTracker->isCancelled()) {
                return OperatorResult::error("Process cancelled");
            }
        }
    }
    QSharedPointer<BitContainer> bitContainer = BitContainer::create(outputBits);
    bitContainer->setName(QString("%1 BER <- %2").arg(ber, 0, 'e', 2).arg(input->name()));

    return OperatorResult::result({bitContainer}, recallablePluginState);
}

QSharedPointer<const OperatorResult> BitsError::getPeriodicErrorBits(QSharedPointer<const BitContainer> input,
                                                                     double ber,
                                                                     const QJsonObject &recallablePluginState,
                                                    QSharedPointer<PluginActionProgress> progressTracker) {
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
            if (progressTracker->isCancelled()) {
                return OperatorResult::error("Process cancelled");
            }
        }
    }

    QSharedPointer<BitContainer> bitContainer = BitContainer::create(outputBits);
    bitContainer->setName(QString("%1 BER <- %2").arg(ber, 0, 'e', 2).arg(input->name()));

    return OperatorResult::result({ bitContainer }, recallablePluginState);
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

QSharedPointer<const OperatorResult> BitsError::operateOnBits(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<PluginActionProgress> progressTracker)
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
