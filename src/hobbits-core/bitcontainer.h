#ifndef BITCONTAINER_H
#define BITCONTAINER_H

#include "frame.h"

#include <QBitArray>
#include <QColor>
#include <QJsonDocument>
#include <QMap>
#include <QObject>
#include <QPixmap>
#include <QSharedPointer>
#include <QUuid>

#include "hobbits-core_global.h"

class PluginActionLineage;
class PluginAction;

class HOBBITSCORESHARED_EXPORT BitContainer : public QObject
{
    Q_OBJECT

    // this lets OperatorActor set the container's ID when necessary
    friend class OperatorActor;

public:
    explicit BitContainer(QObject *parent = nullptr);

    void setFrames(QList<Frame> frames, qint64 maxFrameWidth = -1);

    QList<Frame> getFrames() const;
    qint64 getMaxFrameWidth() const;

    QList<Range> getHighlights(QString type) const;
    void setHighlights(QString type, QList<Range>);

    QStringList getMetadata(QString) const;
    void setMetadata(QString, QStringList);

    QString getName() const;
    void setName(QString name);

    QPixmap getThumbnail();
    QImage getRasterImage(qint64 x, qint64 y, int w, int h) const;
    QImage getByteRasterImage(qint64 x, qint64 y, int w, int h) const;

    QSharedPointer<const BitArray> getBaseBits() const;

    int getFrameOffsetContaining(Range target) const;

    void setActionLineage(QSharedPointer<const PluginActionLineage> lineage);
    QSharedPointer<const PluginActionLineage> getActionLineage() const;

    int getLastFrameOffsetFocus() const;
    int getLastBitOffsetFocus() const;

    bool isRootContainer() const;
    QList<QUuid> getChildUuids() const;
    QList<QUuid> getParentUuids() const;
    QUuid getId() const;
    void detachChild(QUuid childId);
    void detachParent(QUuid parentId);
    void addChild(QUuid childId);
    void addParent(QUuid parentId);

    QJsonDocument serializeJson() const;
    bool deserializeJson(QJsonDocument json);

    friend HOBBITSCORESHARED_EXPORT QDataStream& operator<<(QDataStream&, const BitContainer&);
    friend HOBBITSCORESHARED_EXPORT QDataStream& operator>>(QDataStream&, BitContainer&);

private:
    QString m_name;
    QSharedPointer<BitArray> m_bits;
    QList<Frame> m_frames;
    qint64 m_maxFrameWidth;
    QMap<QString, QList<Range>> m_highlightMap;
    QMap<QString, QStringList> m_metadata;

    QSharedPointer<const PluginActionLineage> m_actionLineage;

    QUuid m_id;
    QList<QUuid> m_children;
    QList<QUuid> m_parents;

    int m_lastFrameOffsetFocus;
    int m_lastBitOffsetFocus;

signals:
    void highlightsChanged(BitContainer *bitContainer);
    void framesChanged(BitContainer *bitContainer);
    void changed(BitContainer *bitContainer);
    void focusRequested(int bitOffset, int frameOffset);

public slots:
    void setBytes(QIODevice *readableBytes, qint64 bitLen = -1);
    void setBytes(QByteArray bytes, qint64 bitLen = -1);
    void setBytes(QSharedPointer<const BitArray> bits);
    void setBytes(QSharedPointer<BitArray> bits);
    void requestFocus(int bitOffset, int frameOffset);
    void recordFocus(int bitOffset, int frameOffset);

    void frameViaHighlights();

};

#endif // BITCONTAINER_H
