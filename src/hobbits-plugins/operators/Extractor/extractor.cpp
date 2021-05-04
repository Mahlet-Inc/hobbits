#include "extractor.h"
#include "extractorform.h"
#include <QObject>

Extractor::Extractor()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"highlight_category", ParameterDelegate::ParameterType::String, false},
        {"highlight_label", ParameterDelegate::ParameterType::String, false},
        {"extract_before", ParameterDelegate::ParameterType::Boolean, true},
        {"extract_highlight", ParameterDelegate::ParameterType::Boolean, true},
        {"extract_after", ParameterDelegate::ParameterType::Boolean, true}
    };

    m_delegate = ParameterDelegateUi::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        bool takeBefore = parameters.value("extract_before").toBool();
                        bool takeHighlight = parameters.value("extract_highlight").toBool();
                        bool takeAfter = parameters.value("extract_after").toBool();
                        QString label = parameters.value("highlight_label").toString();
                        QString prefix = "";
                        if (takeBefore) {
                            prefix = "pre";
                        }
                        if (takeAfter) {
                            if (takeBefore) {
                                prefix += "+";
                            }
                            prefix += "post";
                        }
                        if (!prefix.isEmpty()) {
                            if (takeHighlight) {
                                prefix += "+";
                            }
                            else {
                                prefix += "-";
                            }
                        }
                        return prefix+label;
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new ExtractorForm(delegate);
                    });
}

OperatorInterface* Extractor::createDefaultOperator()
{
    return new Extractor();
}

QString Extractor::name()
{
    return "Extractor";
}

QString Extractor::description()
{
    return "Extract before, within, and/or after highlighted areas in the data";
}

QStringList Extractor::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> Extractor::parameterDelegate()
{
    return m_delegate;
}

int Extractor::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int Extractor::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> Extractor::operateOnBits(
        QList<QSharedPointer<const BitContainer> > inputContainers,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return OperatorResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    if (inputContainers.size() != 1) {
        return OperatorResult::error("Invalid number of input containers (requires 1)");
    }
    QSharedPointer<const BitContainer> inputContainer = inputContainers.at(0);

    bool takeBefore = parameters.value("extract_before").toBool();
    bool takeHighlight = parameters.value("extract_highlight").toBool();
    bool takeAfter = parameters.value("extract_after").toBool();
    QString category = parameters.value("highlight_category").toString();
    QString label = parameters.value("highlight_label").toString();
    auto highlights = inputContainer->info()->highlights(category, label);
    if (highlights.size() < 1) {
        return OperatorResult::error(QString("No highlight found matching required label (%1) and category (%2)").arg(label).arg(category));
    }
    RangeHighlight highlight = highlights.at(0);
    Range range = highlight.range();

    qint64 outputSize = inputContainer->bits()->sizeInBits();
    if (!takeBefore) {
        outputSize -= range.start();
    }
    if (!takeHighlight) {
        outputSize -= range.size();
    }
    if (!takeAfter) {
        outputSize -= (inputContainer->bits()->sizeInBits() - 1 - range.end());
    }
    auto outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
    qint64 outputIndex = 0;
    if (takeBefore) {
        for (qint64 i = 0; i < range.start(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->isCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }
    if (takeHighlight) {
        for (qint64 i = range.start(); i <= range.end(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->isCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }
    if (takeAfter) {
        for (qint64 i = range.end() + 1; i < inputContainer->bits()->sizeInBits(); i++) {
            outBits->set(outputIndex++, inputContainer->bits()->at(i));
            if (i % 5000 == 0) {
                progressTracker->setProgress(outputIndex, outputSize);
                if (progressTracker->isCancelled()) {
                    return OperatorResult::error("Operation cancelled");
                }
            }
        }
    }

    auto outputContainer = BitContainer::create(outBits);

    return OperatorResult::result({outputContainer}, parameters);
}
