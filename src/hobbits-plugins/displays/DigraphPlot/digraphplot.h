#ifndef DIGRAPHPLOT_H
#define DIGRAPHPLOT_H

#include "displayinterface.h"

class DigraphPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.DigraphPlot")
    Q_INTERFACES(DisplayInterface)

public:
    DigraphPlot();

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<DisplayRenderConfig> renderConfig() override;
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) override;
    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QImage renderDisplay(
            QSize viewportSize,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

    QImage renderOverlay(
            QSize viewportSize,
            const QJsonObject &parameters) override;

private:
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;
};

#endif // DIGRAPHPLOT_H
