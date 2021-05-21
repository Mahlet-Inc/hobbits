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

DisplayResult *DisplayResult::setParameters(const Parameters &parameters)
{
    m_parameters = parameters;
    return this;
}

const Parameters DisplayResult::parameters() const
{
    return m_parameters;
}

bool DisplayResult::hasNullParameters() const
{
    return m_parameters.isNull();
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

QSharedPointer<DisplayResult> DisplayResult::result(const QImage& image, Parameters parameters)
{
    return QSharedPointer<DisplayResult>((new DisplayResult())->setImage(image)->setParameters(parameters));
}
