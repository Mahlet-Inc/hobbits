#ifndef PREVIEWSCROLLBAR_H
#define PREVIEWSCROLLBAR_H

#include <QWidget>
#include <QFuture>
#include "bitcontainermanager.h"
#include "actionwatcher.h"
#include "actionrenderprogress.h"
#include "displayhandle.h"

class HOBBITSCORESHARED_EXPORT PreviewScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewScrollBar(QWidget *parent = nullptr);
    int getFrameOffset();

    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void frameOffsetChanged(int);

public slots:
    void setFrameOffset(int);
    void setBitContainerManager(QSharedPointer<BitContainerManager> manager);
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle);

private slots:
    void newContainer();
    void checkRenderWatchers();
    void checkDisplayHandleOffset();

private:
    void getOffsetFromEvent(QMouseEvent* event);
    static QImage renderPreview(QSharedPointer<BitContainer> bits, QSharedPointer<ActionRenderProgress> progress);

    int m_frameOffset = 0;
    bool m_mousePressing = false;
    QSharedPointer<BitContainerManager> m_manager;
    QSharedPointer<DisplayHandle> m_displayHandle;

    QHash<quint64, QWeakPointer<BitContainer>> m_weakRefMap;
    QHash<quint64, QImage> m_imageCache;

    QHash<QSharedPointer<BitContainer>, QSharedPointer<ActionWatcher<QImage>>> m_renderWatchers;
};

#endif // PREVIEWSCROLLBAR_H
