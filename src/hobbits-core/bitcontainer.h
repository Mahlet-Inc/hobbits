#ifndef BITCONTAINER_H
#define BITCONTAINER_H

#include "frame.h"
#include <QJsonDocument>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QUuid>
#include "bitinfo.h"
#include "hobbits-core_global.h"
#include <QMutex>

class PluginActionLineage;
class PluginAction;

/**
  * @brief The BitContainer class manages bitwise data and the metadata associated with it
  *
  * The BitContainer class is mostly a simple wrapper around the BitArray and BitInfo classes
  * which provide bitwise data and metadata management respectively. It also maintains a unique
  * ID, parent-child relationships, and a PluginActionLineage for reconstructing the steps that
  * were taken to create the BitContainer.
  *
  * \see BitArray BitInfo PluginActionLineage
*/
class HOBBITSCORESHARED_EXPORT BitContainer : public QObject
{
    Q_OBJECT

    // this lets OperatorActor set the container's ID when necessary
    friend class OperatorActor;

public:
    static QSharedPointer<BitContainer> create(QByteArray bytes, qint64 bitLen = -1, QSharedPointer<const BitInfo> info=QSharedPointer<const BitInfo>());
    static QSharedPointer<BitContainer> create(QIODevice *readableBytes, qint64 bitLen = -1, QSharedPointer<const BitInfo> info=QSharedPointer<const BitInfo>());
    static QSharedPointer<BitContainer> create(QSharedPointer<const BitArray> bits, QSharedPointer<const BitInfo> info=QSharedPointer<const BitInfo>());
    static QSharedPointer<BitContainer> create(QSharedPointer<BitArray> bits, QSharedPointer<const BitInfo> info=QSharedPointer<const BitInfo>());

    void setInfo(QSharedPointer<const BitInfo> info);

    QSharedPointer<const BitArray> bits() const;
    QSharedPointer<const BitInfo> info() const;
    QSharedPointer<BitInfo> info();

    Frame frameAt(qint64 i) const;
    qint64 frameCount() const;
    qint64 maxFrameWidth() const;

    qint64 size() const;

    QString name() const;
    void setName(QString name);
    bool nameWasSet() const;

    void setActionLineage(QSharedPointer<PluginActionLineage> lineage);
    QSharedPointer<const PluginActionLineage> actionLineage() const;
    QSharedPointer<PluginActionLineage> actionLineage();

    bool isRootContainer() const;
    QList<QUuid> childUuids() const;
    QList<QUuid> parentUuids() const;
    QUuid id() const;
    void detachChild(QUuid childId);
    void detachParent(QUuid parentId);
    void addChild(QUuid childId);
    void addParent(QUuid parentId);

    static QSharedPointer<BitContainer> deserialize(QDataStream &stream);
    void serialize(QDataStream &stream) const;

    

private:
    explicit BitContainer();

    QString m_name;
    bool m_nameWasSet;
    QSharedPointer<BitArray> m_bits;
    QSharedPointer<BitInfo> m_info;

    QSharedPointer<PluginActionLineage> m_actionLineage;

    QUuid m_id;
    QList<QUuid> m_children;
    QList<QUuid> m_parents;
    QMutex m_mutex;

Q_SIGNALS:
    void changed();
};

#endif // BITCONTAINER_H
