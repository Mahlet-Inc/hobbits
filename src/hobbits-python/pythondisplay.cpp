#include "pythondisplay.h"
#include <QImage>
#include "displayhandle.h"
#include <QTemporaryDir>
#include "pythonarg.h"
#include "hobbitspython.h"

PythonDisplay::PythonDisplay(QSharedPointer<PythonPluginConfig> config) :
    m_config(config)
{
}

DisplayInterface *PythonDisplay::createDefaultDisplay()
{
    return new PythonDisplay(m_config);
}

QString PythonDisplay::name()
{
    return m_config->name();
}

QString PythonDisplay::description()
{
    return m_config->description();
}

QStringList PythonDisplay::tags()
{
    return m_config->tags();
}

QSharedPointer<DisplayRenderConfig> PythonDisplay::renderConfig()
{
    return m_config->renderConfig();
}

void PythonDisplay::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    // TODO: configurable display handle connections for python displays
}

QSharedPointer<ParameterDelegate> PythonDisplay::parameterDelegate()
{
    return m_config->delegate();
}

QImage PythonDisplay::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    if (m_handle.isNull()
            || m_handle->currentContainer().isNull()
            || !m_config->delegate()->validate(parameters)) {
        return QImage();
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return QImage();
    }

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return QImage();
    }
    userScriptFile.write(m_config->script().toLatin1());
    userScriptFile.close();

    int imageBytes = viewportSize.width() * viewportSize.height() * 4;
    char* byteBuffer = new char[imageBytes];
    auto pyRequest = PythonRequest::create(userScriptFile.fileName())->setFunctionName("render_display");
    for (auto extraPath: m_config->extraPaths()) {
        pyRequest->addPathExtension(extraPath);
    }
    pyRequest->addArg(PythonArg::displayHandle(m_handle));
    pyRequest->addArg(PythonArg::imageBuffer(byteBuffer, viewportSize));
    for (auto param : m_config->parameterInfos()) {
        if (param.type == QJsonValue::String) {
            pyRequest->addArg(PythonArg::qString(parameters.value(param.name).toString()));
        }
        else if (param.type == QJsonValue::Double) {
            if (m_config->parameterNumberType(param.name) == PythonPluginConfig::Integer) {
                pyRequest->addArg(PythonArg::integer(parameters.value(param.name).toInt()));
            }
            else {
                pyRequest->addArg(PythonArg::number(parameters.value(param.name).toDouble()));
            }
        }
        else if (param.type == QJsonValue::Bool) {
            pyRequest->addArg(PythonArg::boolean(parameters.value(param.name).toBool()));
        }
    }

    // If there are several render requests in quick succession, this will prevent all but the most recent from running their script
    HobbitsPython::waitForInterpreterLock();
    if (progress->isCancelled()) {
        delete[] byteBuffer;
        return QImage();
    }

    auto watcher = HobbitsPython::getInstance().runProcessScript(pyRequest, progress);
    watcher->watcher()->future().waitForFinished();
    auto result = watcher->result();

    if (progress->isCancelled()) {
        delete[] byteBuffer;
        return QImage();
    }

    QString output = "";
    bool error = false;
    if (!result->getStdOut().isEmpty()) {
        output += "Python stdout:\n" + result->getStdOut() + "\n\n";
    }
    if (!result->getStdErr().isEmpty()) {
        error = true;
        output += "Python stderr:\n" + result->getStdErr() + "\n\n";
    }
    if (!result->errors().isEmpty()) {
        error = true;
        output += "Other errors:\n" + result->errors().join("\n") + "\n\n";
    }
    if (error) {
        delete[] byteBuffer;
        return QImage();
    }

    return QImage(reinterpret_cast<uchar*>(byteBuffer), viewportSize.width(), viewportSize.height(), QImage::Format_ARGB32, [](void *info) {
        delete[] reinterpret_cast<char*>(info);
    }, byteBuffer);
}

QImage PythonDisplay::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    Q_UNUSED(viewportSize)
    Q_UNUSED(parameters)

    // TODO: display overlays and interactivity for python displays

    return QImage();
}
