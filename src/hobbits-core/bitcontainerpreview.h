#ifndef BITCONTAINERPREVIEW_H
#define BITCONTAINERPREVIEW_H

#include "bitcontainer.h"
#include <QObject>

/// A proxy class for BitContainers that hides bit editing functionality
class BitContainerPreview : public QObject
{
    Q_OBJECT

public:
    explicit BitContainerPreview(QSharedPointer<BitContainer> m_bitContainer);

    QList<Frame> getFrames() const;
    int getMaxFrameWidth() const;

    QList<Range> getHighlights(QString type) const;
    void setHighlights(QString type, QList<Range>);

    QStringList getMetadata(QString) const;
    void setMetadata(QString, QStringList);

    QString getName() const;
    void setName(QString name);

    QPixmap getThumbnail() const;
    QImage getRasterImage(int x, int y, int w, int h) const;

    QSharedPointer<const BitArray> getBaseBits() const;

    int getFrameOffsetContaining(Range target) const;

    int getLastFrameOffsetFocus() const;
    int getLastBitOffsetFocus() const;

signals:
    void highlightsChanged(BitContainer *bitContainer);
    void framesChanged(BitContainer *bitContainer);
    void changed(BitContainer *bitContainer);
    void focusRequested(int bitOffset, int frameOffset);

public slots:
    void requestFocus(int bitOffset, int frameOffset);

private:
    QSharedPointer<BitContainer> m_bitContainer;
};

#endif // BITCONTAINERPREVIEW_H
