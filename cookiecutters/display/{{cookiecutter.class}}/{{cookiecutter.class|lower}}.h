#ifndef {{cookiecutter.class|upper}}
#define {{cookiecutter.class|upper}}

#include "displayinterface.h"

class {{cookiecutter.class}} : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.{{cookiecutter.class}}")
    Q_INTERFACES(DisplayInterface)

public:
    {{cookiecutter.class}}();

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
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;
};

#endif // {{cookiecutter.class|upper}}
