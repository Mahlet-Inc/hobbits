#ifndef PYTHONDISPLAY_H
#define PYTHONDISPLAY_H

#include "displayinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonDisplay : public DisplayInterface
{
public:
    PythonDisplay(QSharedPointer<PythonPluginConfig> config);

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<DisplayRenderConfig> renderConfig() override;
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) override;
    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<DisplayResult> renderDisplay(
            QSize viewportSize,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

    QSharedPointer<DisplayResult> renderOverlay(
            QSize viewportSize,
            const QJsonObject &parameters) override;

private:
    QSharedPointer<PythonPluginConfig> m_config;
    QSharedPointer<DisplayHandle> m_handle;
};

#endif // PYTHONDISPLAY_H
