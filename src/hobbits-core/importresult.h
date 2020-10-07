#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include "bitcontainer.h"
#include <QJsonObject>

/**
  * @brief The ExportResult class contains the result of an ImportExportInterface::importBits() run
  *
  * \see ImportExportInterface
*/
class HOBBITSCORESHARED_EXPORT ImportResult
{
public:
    ImportResult();

    ImportResult* setContainer(QSharedPointer<BitContainer> container);
    QSharedPointer<BitContainer> getContainer() const;

    ImportResult* setParameters(QJsonObject parameters);
    const QJsonObject parameters() const;

    bool hasEmptyParameters() const;
    QString errorString() const;

    static QSharedPointer<ImportResult> nullResult();
    static QSharedPointer<ImportResult> error(QString error);
    static QSharedPointer<ImportResult> result(QSharedPointer<BitContainer> container, QJsonObject parameters);

private:
    QSharedPointer<BitContainer> m_container;
    QJsonObject m_parameters;
    QString m_errorString;
};

#endif // IMPORTRESULT_H
