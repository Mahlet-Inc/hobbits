#ifndef DISPLAYRESULT_H
#define DISPLAYRESULT_H

#include <parameters.h>
#include <QImage>
#include <QSharedPointer>
#include "hobbits-widgets_global.h"

/**
  * @brief The DisplayResult class contains the result of a DisplayInterface::renderDisplay() or DisplayInterface::renderOverlay() run
  *
  * \see DisplayInterface
*/
class HOBBITSWIDGETSSHARED_EXPORT DisplayResult
{
public:
    DisplayResult();

    DisplayResult* setImage(const QImage& image);
    QImage getImage() const;

    DisplayResult* setParameters(const Parameters &parameters);
    const Parameters parameters() const;

    bool hasNullParameters() const;
    QString errorString() const;

    static QSharedPointer<DisplayResult> nullResult();
    static QSharedPointer<DisplayResult> error(QString error);
    static QSharedPointer<DisplayResult> result(const QImage& image, Parameters parameters);

private:
    QImage m_image;
    Parameters m_parameters;
    QString m_errorString;

};

#endif // DISPLAYRESULT_H
