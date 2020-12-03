#ifndef FREQUENCYPLOT_H
#define FREQUENCYPLOT_H

#include "displayinterface.h"

class FrequencyPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.FrequencyPlot")
    Q_INTERFACES(DisplayInterface)

public:
    FrequencyPlot();

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

    QMap<int, quint64> m_barMax;
    QVector<int> m_barFrequencies;
};

#endif // FREQUENCYPLOT_H
