#include "pythonrequest.h"

PythonRequest::PythonRequest()
{

}

QList<PythonArg *> PythonRequest::args() const
{
    return m_args;
}

PythonRequest::~PythonRequest()
{
    for (auto arg : m_args) {
        delete arg;
    }
    m_args.clear();
}

QSharedPointer<PythonRequest> PythonRequest::create(QString scriptName)
{
    QSharedPointer<PythonRequest> request(new PythonRequest());
    request->m_scriptName = scriptName;
    return request;
}

QSharedPointer<PythonRequest> PythonRequest::setFunctionName(QString functionName)
{
    this->m_functionName = functionName;
    return sharedFromThis();
}

QSharedPointer<PythonRequest> PythonRequest::addPathExtension(QString extension)
{
    m_pathExtensions.append(extension);
    return sharedFromThis();
}

QSharedPointer<PythonRequest> PythonRequest::addArg(PythonArg *arg)
{
    m_args.append(arg);
    return sharedFromThis();
}

QStringList PythonRequest::pathExtensions() const
{
    return m_pathExtensions;
}

QString PythonRequest::scriptName() const
{
    return m_scriptName;
}

QString PythonRequest::functionName() const
{
    return m_functionName;
}
