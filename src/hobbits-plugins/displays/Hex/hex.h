#ifndef HEX_H
#define HEX_H

#include "displayinterface.h"

class Hex : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.Hex")
    Q_INTERFACES(DisplayInterface)

public:
    Hex();

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<DisplayRenderConfig> renderConfig() override;
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) override;
    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<DisplayResult> renderDisplay(
            QSize viewportSize,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

    QSharedPointer<DisplayResult> renderOverlay(
            QSize viewportSize,
            const Parameters &parameters) override;

private:
    QPoint headerOffset(const Parameters &parameters);
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;
    Parameters m_lastParams;
};

#endif // HEX_H
