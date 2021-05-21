#ifndef EXPORTRESULT_H
#define EXPORTRESULT_H

#include "bitcontainer.h"
#include "parameters.h"

/**
  * @brief The ExportResult class contains the result of an ImportExportInterface::exportBits() run
  *
  * \see ImportExportInterface
*/
class HOBBITSCORESHARED_EXPORT ExportResult
{
public:
    ExportResult();

    ExportResult* setParameters(const Parameters &parameters);
    const Parameters parameters() const;

    bool hasNullParameters() const;
    QString errorString() const;

    static QSharedPointer<ExportResult> nullResult();
    static QSharedPointer<ExportResult> error(QString errorString);
    static QSharedPointer<ExportResult> result(Parameters parameters);

private:
    Parameters m_parameters;
    QString m_errorString;
};

#endif // EXPORTRESULT_H
