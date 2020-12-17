#ifndef HILBERTPLOT_H
#define HILBERTPLOT_H

#include "displayinterface.h"
#include <QMap>
#include <QPoint>

class HilbertPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.HilbertPlot")
    Q_INTERFACES(DisplayInterface)

public:
    HilbertPlot();

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
    void rotate(QPoint &p, int n, bool rx, bool ry);
    QPoint toHilbertCoordinate(int n, int idx);
    QVector<QPoint> getPointsForOrder(int order);

    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;

    QMap<int, QVector<QPoint>> m_hilbertPoints;
    QList<int> m_hilbertRecursionOrders;
};

#endif // HILBERTPLOT_H
