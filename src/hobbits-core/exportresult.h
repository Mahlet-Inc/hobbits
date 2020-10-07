#ifndef EXPORTRESULT_H
#define EXPORTRESULT_H

#include "bitcontainer.h"
#include <QJsonObject>

/**
  * @brief The ExportResult class contains the result of an ImportExportInterface::exportBits() run
  *
  * \see ImportExportInterface
*/
class HOBBITSCORESHARED_EXPORT ExportResult
{
public:
    ExportResult();

    ExportResult* setParameters(QJsonObject parameters);
    const QJsonObject parameters() const;

    bool hasEmptyParameters() const;
    QString errorString() const;

    static QSharedPointer<ExportResult> nullResult();
    static QSharedPointer<ExportResult> error(QString errorString);
    static QSharedPointer<ExportResult> result(QJsonObject parameters);

private:
    QJsonObject m_parameters;
    QString m_errorString;
};

#endif // EXPORTRESULT_H
