#include "algorithm"
#include "cmath"
#include "math.h"
#include "widthframer.h"
#include "widthframerform.h"

WidthFramer::WidthFramer()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"width", QJsonValue::Double}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        int width = parameters.value("width").toInt();

                        return QString("Set Frame Width to %1").arg(width);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new WidthFramerForm(delegate);
                    });
}

AnalyzerInterface* WidthFramer::createDefaultAnalyzer()
{
    return new WidthFramer();
}

QString WidthFramer::name()
{
    return "Width Framer";
}

QString WidthFramer::description()
{
    return "Frame data to a constant bit width";
}

QStringList WidthFramer::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> WidthFramer::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> WidthFramer::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    if (!m_delegate->validate(parameters)) {
        return AnalyzerResult::error("Invalid parameters passed to Width Framer");
    }
    progress->setProgressPercent(10);

    qint64 frameWidth = parameters.value("width").toInt();
    QSharedPointer<const BitArray> bits = container->bits();
    QSharedPointer<BitInfo> bitInfo = BitInfo::copyFromContainer(container);
    bitInfo->setFrames(RangeSequence::fromConstantSize(frameWidth, bits->sizeInBits()));
    progress->setProgressPercent(50);
    return AnalyzerResult::result(bitInfo, parameters);
}
