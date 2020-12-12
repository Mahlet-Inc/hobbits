#ifndef ASCII_H
#define ASCII_H

#include "displayinterface.h"

class Ascii : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.Ascii")
    Q_INTERFACES(DisplayInterface)

public:
    Ascii();

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
    QPoint headerOffset(const QJsonObject &parameters);
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;
    QJsonObject m_lastParams;
};

#endif // ASCII_H
