#ifndef BITCONTAINERMANAGER_H
#define BITCONTAINERMANAGER_H

#include "bitcontainer.h"
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitContainerManager : public QObject
{
    Q_OBJECT

public:
    explicit BitContainerManager(QObject *parent = nullptr);
    virtual ~BitContainerManager();

    virtual QSharedPointer<BitContainer> getCurrentContainer();
    virtual QSharedPointer<BitContainer> getContainerById(QUuid id);

    virtual bool addContainer(QSharedPointer<BitContainer> container);
    virtual bool selectContainer(QSharedPointer<BitContainer> container);

    virtual void deleteCurrentContainer();
    virtual void deleteAllContainers();

Q_SIGNALS:
    void currSelectionChanged(QSharedPointer<BitContainer> selected, QSharedPointer<BitContainer> deselected);
    void containerAdded(QSharedPointer<BitContainer>);

protected:
    QMap<QUuid, QSharedPointer<BitContainer>> m_containerMap;
    QSharedPointer<BitContainer> m_current;
};

#endif // BITCONTAINERMANAGER_H
