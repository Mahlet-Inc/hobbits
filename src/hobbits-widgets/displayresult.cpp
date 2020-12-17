#include "displayresult.h"

DisplayResult::DisplayResult()
{

}

DisplayResult *DisplayResult::setImage(const QImage& image)
{
    m_image = image;
    return this;
}

QImage DisplayResult::getImage() const
{
    return m_image;
}

DisplayResult *DisplayResult::setParameters(QJsonObject parameters)
{
    m_parameters = parameters;
    return this;
}

const QJsonObject DisplayResult::parameters() const
{
    return m_parameters;
}

bool DisplayResult::hasEmptyParameters() const
{
    return m_parameters.isEmpty();
}

QString DisplayResult::errorString() const
{
    return m_errorString;
}

QSharedPointer<DisplayResult> DisplayResult::nullResult()
{
    return QSharedPointer<DisplayResult>(new DisplayResult());
}

QSharedPointer<DisplayResult> DisplayResult::error(QString errorString)
{
    auto result = new DisplayResult();
    result->m_errorString = errorString;
    return QSharedPointer<DisplayResult>(result);
}

QSharedPointer<DisplayResult> DisplayResult::result(const QImage& image, QJsonObject parameters)
{
    return QSharedPointer<DisplayResult>((new DisplayResult())->setImage(image)->setParameters(parameters));
}
