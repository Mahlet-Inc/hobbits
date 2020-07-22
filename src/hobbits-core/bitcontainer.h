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
#include "bitinfo.h"
#include "hobbits-core_global.h"
#include <QMutex>

class PluginActionLineage;
class PluginAction;

class HOBBITSCORESHARED_EXPORT BitContainer : public QObject
{
    Q_OBJECT

    // this lets OperatorActor set the container's ID when necessary
    friend class OperatorActor;

public Q_SLOTS:
    void setBits(QIODevice *readableBytes, qint64 bitLen = -1, QSharedPointer<BitInfo> bitInfo=QSharedPointer<BitInfo>());
    void setBits(QByteArray bytes, qint64 bitLen = -1, QSharedPointer<BitInfo> bitInfo=QSharedPointer<BitInfo>());
    void setBits(QSharedPointer<const BitArray> bits, QSharedPointer<BitInfo> bitInfo=QSharedPointer<BitInfo>());
    void setBits(QSharedPointer<BitArray> bits, QSharedPointer<BitInfo> bitInfo=QSharedPointer<BitInfo>());
    void setBitInfo(QSharedPointer<BitInfo>);

public:
    explicit BitContainer(QObject *parent = nullptr);

    QSharedPointer<const BitArray> bits() const;
    QSharedPointer<const BitInfo> bitInfo() const;
    QSharedPointer<BitInfo> bitInfo();

    QVector<Frame> frames() const;
    qint64 maxFrameWidth() const;

    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

    QString name() const;
    void setName(QString name);
    bool nameWasSet() const;

    void setActionLineage(QSharedPointer<PluginActionLineage> lineage);
    QSharedPointer<const PluginActionLineage> getActionLineage() const;
    QSharedPointer<PluginActionLineage> getActionLineage();

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
    bool m_nameWasSet;
    QSharedPointer<BitArray> m_bits;
    QSharedPointer<BitInfo> m_bitInfo;

    QSharedPointer<PluginActionLineage> m_actionLineage;

    QUuid m_id;
    QList<QUuid> m_children;
    QList<QUuid> m_parents;
    QMutex m_mutex;

Q_SIGNALS:
    void changed();
};

#endif // BITCONTAINER_H
