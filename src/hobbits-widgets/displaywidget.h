#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include "displayhandle.h"
#include "displayinterface.h"
#include "actionrenderprogress.h"
#include "abstractparametereditor.h"

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QSharedPointer<DisplayInterface> display,
                           QSharedPointer<DisplayHandle> handle,
                           QWidget *parent = nullptr);
    ~DisplayWidget() override;

    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    static QImage render(QSharedPointer<DisplayInterface> display,
                         QSize viewportSize,
                         const QJsonObject &parameters,
                         QSharedPointer<PluginActionProgress> progress);

    QJsonObject displayParameters() const;
    void setDisplayParameters(const QJsonObject &displayParameters);

    QSharedPointer<DisplayInterface> display() const;

    QSharedPointer<DisplayHandle> handle() const;

signals:
    void aboutToRedraw();

public slots:

private slots:
    void performDisplayRender();
    void handleDisplayRenderPreview(QString type, QVariant value);
    void setDisplayImage(QImage image);
    void resetRendering();
    void fullRedraw(bool newContainer = false);
    void showContextMenu(const QPoint &point);

    void checkNewContainer();
    void checkFullRedraw(DisplayInterface* display = nullptr, bool newContainer = false);
    void checkOverlayRedraw(DisplayInterface* display = nullptr);

private:
    QSharedPointer<DisplayInterface> m_display;
    QSharedPointer<DisplayHandle> m_handle;
    QJsonObject m_displayParameters;
    QImage m_displayImage;
    bool m_redrawing;

    QFutureWatcher<QImage> m_displayRenderWatcher;
    QSharedPointer<PluginActionProgress> m_displayRenderProgress;
};

#endif // DISPLAYWIDGET_H
