#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include "displayhandle.h"
#include "displayinterface.h"
#include "actionrenderprogress.h"
#include "abstractparametereditor.h"
#include "displayresult.h"

class HOBBITSWIDGETSSHARED_EXPORT DisplayWidget : public QWidget
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

    static QSharedPointer<DisplayResult> render(QSharedPointer<DisplayInterface> display,
                         QSize viewportSize,
                         const Parameters &parameters,
                         QSharedPointer<PluginActionProgress> progress);

    Parameters displayParameters() const;
    void setDisplayParameters(const Parameters &displayParameters);

    QSharedPointer<DisplayInterface> display() const;

    QSharedPointer<DisplayHandle> handle() const;

signals:
    void hasNewContainer();

public slots:

private slots:
    void performDisplayRender();
    void handleDisplayRenderPreview(QString type, QVariant value);
    void setDisplayResult(QSharedPointer<DisplayResult> result);
    void resetRendering();
    void fullRedraw();
    void showContextMenu(const QPoint &point);

    void checkNewContainer();
    void checkFullRedraw(DisplayInterface* display = nullptr);
    void checkOverlayRedraw(DisplayInterface* display = nullptr);

    void scheduleRepaint();

    void drawError(QPainter* painter, QString error);

private:
    QSharedPointer<DisplayInterface> m_display;
    QSharedPointer<DisplayHandle> m_handle;
    Parameters m_displayParameters;
    QSharedPointer<DisplayResult> m_displayResult;
    bool m_repaintScheduled;
    QMutex m_mutex;

    QSharedPointer<QFutureWatcher<QSharedPointer<DisplayResult>>> m_displayRenderWatcher;
    QSharedPointer<PluginActionProgress> m_displayRenderProgress;
};

#endif // DISPLAYWIDGET_H
