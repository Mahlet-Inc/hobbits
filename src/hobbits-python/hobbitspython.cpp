#include "hobbitspython.h"
#include "pythoninterpreter.h"
#include <QtConcurrent/QtConcurrent>

HobbitsPython::HobbitsPython()
{
}

HobbitsPython& HobbitsPython::getInstance()
{
    static HobbitsPython instance;
    return instance;
}

QString HobbitsPython::pythonVersion()
{
#ifdef PYTHON_LINK_LIB
    QString version = PYTHON_LINK_LIB;
#else
    QString version = "Unknown Version";
#endif
    return version;
}

QSharedPointer<PluginActionWatcher<QSharedPointer<PythonResult> > > HobbitsPython::runProcessScript(
        QSharedPointer<PythonRequest> request,
        bool includeActionProgressArg)
{
    if (includeActionProgressArg) {
        return runProcessScript(request, QSharedPointer<PluginActionProgress>(new PluginActionProgress()));
    }
    else {
        return runProcessScript(request, QSharedPointer<PluginActionProgress>());
    }
}

QSharedPointer<PluginActionWatcher<QSharedPointer<PythonResult> > > HobbitsPython::runProcessScript(QSharedPointer<PythonRequest> request, QSharedPointer<PluginActionProgress> progress)
{
    if (progress.isNull()) {
        progress = QSharedPointer<PluginActionProgress>(new PluginActionProgress());
    }
    else {
        request->addArg(PythonArg::actionProgress(progress));
    }

    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            PythonInterpreter::runProcessScript,
            request);

    QSharedPointer<PluginActionWatcher<QSharedPointer<PythonResult>>> watcher(
            new PluginActionWatcher<QSharedPointer<PythonResult>>(future, progress));

    return watcher;
}

void HobbitsPython::recursiveDirCopy(QString src, QString dest)
{
    QDir srcDir(src);
    if (!srcDir.exists()) {
        return;
    }

    QDir destDir(dest);
    if (!destDir.exists()) {
        destDir.mkpath(".");
    }

    for (QString subDir : srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        recursiveDirCopy(srcDir.filePath(subDir), destDir.filePath(subDir));
    }

    for (QString file : srcDir.entryList(QDir::Files)) {
        QFile::copy(srcDir.filePath(file), destDir.filePath(file));
    }
}
