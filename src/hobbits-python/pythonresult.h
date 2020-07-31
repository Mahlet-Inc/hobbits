#ifndef PYTHONRESULT_H
#define PYTHONRESULT_H

#include <QString>
#include <QFile>
#include <QSharedPointer>

#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonResult
{
public:
    static QSharedPointer<PythonResult> result(QStringList errors);
    static QSharedPointer<PythonResult> result(QFile &stdoutFile, QFile &stderrFile, QStringList errors);

    QString stdout() const;
    QString stderr() const;
    QStringList errors() const;

private:
    PythonResult();
    QString m_stdout;
    QString m_stderr;
    QStringList m_errors;
};

#endif // PYTHONRESULT_H
