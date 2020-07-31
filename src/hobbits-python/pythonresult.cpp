#include "pythonresult.h"

PythonResult::PythonResult()
{

}

QStringList PythonResult::errors() const
{
    return m_errors;
}

QString PythonResult::stderr() const
{
    return m_stderr;
}

QString PythonResult::stdout() const
{
    return m_stdout;
}

QSharedPointer<PythonResult> PythonResult::result(QStringList errors)
{
    auto r = QSharedPointer<PythonResult>(new PythonResult());
    r->m_errors = errors;
    return r;
}

QSharedPointer<PythonResult> PythonResult::result(QFile &stdoutFile, QFile &stderrFile, QStringList errors)
{
    auto r = QSharedPointer<PythonResult>(new PythonResult());
    r->m_errors = errors;
    if (!stdoutFile.open(QFile::ReadOnly)) {
        r->m_errors.append(QString("Failed to open stdout file '%1'").arg(stdoutFile.fileName()));
    }
    else {
        r->m_stdout = stdoutFile.readAll();
    }
    stdoutFile.close();

    if (!stderrFile.open(QFile::ReadOnly)) {
        r->m_errors.append(QString("Failed to open stderr file '%1'").arg(stderrFile.fileName()));
    }
    else {
        r->m_stderr = stderrFile.readAll();
    }
    stderrFile.close();

    return r;
}
