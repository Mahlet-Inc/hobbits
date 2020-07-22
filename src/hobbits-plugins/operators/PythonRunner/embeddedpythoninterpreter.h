#ifndef EMBEDDEDPYTHONINTERPRETER_H
#define EMBEDDEDPYTHONINTERPRETER_H

#include <QTemporaryDir>
#include <QSharedPointer>
#include "bitarray.h"

class EmbeddedPythonInterpreter
{
public:
    EmbeddedPythonInterpreter();

    void runScript(QString scriptFileName, QSharedPointer<const BitArray> inputBitArray, QSharedPointer<BitArray> outputBitArray);

    QString readErrors();

private:
    void writeError(QString error);

    QTemporaryDir m_workingDir;
    QString m_errorFile;
};

#endif // EMBEDDEDPYTHONINTERPRETER_H
