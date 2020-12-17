#ifndef BYTERASTER_H
#define BYTERASTER_H

#include "displayinterface.h"

class ByteRaster : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.ByteRaster")
    Q_INTERFACES(DisplayInterface)

public:
    ByteRaster();

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
    QPoint headerOffset(const QJsonObject &parameters);
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;
    QJsonObject m_lastParams;
};

#endif // BYTERASTER_H
