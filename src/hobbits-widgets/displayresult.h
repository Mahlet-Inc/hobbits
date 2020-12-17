#ifndef DISPLAYRESULT_H
#define DISPLAYRESULT_H

#include <QJsonObject>
#include <QImage>
#include "hobbits-core_global.h"

/**
  * @brief The DisplayResult class contains the result of a DisplayInterface::renderDisplay() or DisplayInterface::renderOverlay() run
  *
  * \see DisplayInterface
*/
class HOBBITSCORESHARED_EXPORT DisplayResult
{
public:
    DisplayResult();

    DisplayResult* setImage(const QImage& image);
    QImage getImage() const;

    DisplayResult* setParameters(QJsonObject parameters);
    const QJsonObject parameters() const;

    bool hasEmptyParameters() const;
    QString errorString() const;

    static QSharedPointer<DisplayResult> nullResult();
    static QSharedPointer<DisplayResult> error(QString error);
    static QSharedPointer<DisplayResult> result(const QImage& image, QJsonObject parameters);

private:
    QImage m_image;
    QJsonObject m_parameters;
    QString m_errorString;

};

#endif // DISPLAYRESULT_H
