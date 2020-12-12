#ifndef PREVIEWSCROLLBAR_H
#define PREVIEWSCROLLBAR_H

#include <QWidget>
#include <QFuture>
#include "bitcontainermanager.h"
#include "pluginactionwatcher.h"
#include "actionrenderprogress.h"
#include "displayhandle.h"
#include "hobbits-widgets_global.h"
#include "displaywidget.h"

class HOBBITSWIDGETSSHARED_EXPORT PreviewScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewScrollBar(QWidget *parent = nullptr);
    ~PreviewScrollBar() override;
    qint64 getFrameOffset();

    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void frameOffsetChanged(qint64);

public slots:
    void setFrameOffset(qint64);
    void setBitContainerManager(QSharedPointer<BitContainerManager> manager);
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle);

private slots:
    void newContainer();
    void checkDisplayHandleOffset();
    void checkDisplayRenderRange(Range range);
    void checkActiveDisplays(QSet<DisplayWidget*> activeDisplays);

private:
    void getOffsetFromEvent(QMouseEvent* event);
    static QImage renderPreview(QSharedPointer<BitContainer> bits, QSharedPointer<ActionRenderProgress> progress);

    Range m_renderedRange;
    qint64 m_frameOffset = 0;
    bool m_mousePressing = false;
    QSharedPointer<BitContainerManager> m_manager;
    QSharedPointer<DisplayHandle> m_displayHandle;

    QHash<quint64, QWeakPointer<BitContainer>> m_weakRefMap;
    QHash<quint64, QImage> m_imageCache;
    QHash<quint64, QImage> m_previewImageCache;

    QHash<quint64, QSharedPointer<PluginActionWatcher<QImage>>> m_renderWatchers;
};

#endif // PREVIEWSCROLLBAR_H
