#ifndef PYTHONREQUEST_H
#define PYTHONREQUEST_H

#include <QSharedPointer>
#include "bitarray.h"
#include "pythonarg.h"

class HOBBITSPYTHONSHARED_EXPORT PythonRequest : public QEnableSharedFromThis<PythonRequest>
{
public:
    ~PythonRequest();
    static QSharedPointer<PythonRequest> create(QString scriptName);

    QSharedPointer<PythonRequest> setFunctionName(QString functionName);
    QSharedPointer<PythonRequest> addPathExtension(QString extension);
    QSharedPointer<PythonRequest> addArg(PythonArg *arg);


    QStringList pathExtensions() const;
    QString scriptName() const;
    QString functionName() const;
    QList<PythonArg *> args() const;

private:
    PythonRequest();
    QString m_scriptName;
    QString m_functionName;
    QStringList m_pathExtensions;
    QList<PythonArg*> m_args;
};

#endif // PYTHONREQUEST_H
