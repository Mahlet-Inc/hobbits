#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include "bitcontainer.h"
#include "parameters.h"

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

    ImportResult* setParameters(const Parameters &parameters);
    const Parameters parameters() const;

    bool hasNullParameters() const;
    QString errorString() const;

    static QSharedPointer<ImportResult> nullResult();
    static QSharedPointer<ImportResult> error(QString error);
    static QSharedPointer<ImportResult> result(QSharedPointer<BitContainer> container, Parameters parameters);

private:
    QSharedPointer<BitContainer> m_container;
    Parameters m_parameters;
    QString m_errorString;
};

#endif // IMPORTRESULT_H
