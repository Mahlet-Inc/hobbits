#include "bitcontainermanager.h"

BitContainerManager::BitContainerManager(QObject *parent) :
    QObject(parent)
{
}

BitContainerManager::~BitContainerManager()
{

}

QSharedPointer<BitContainer> BitContainerManager::currentContainer()
{
    return m_current;
}

QSharedPointer<BitContainer> BitContainerManager::getContainerById(QUuid id)
{
    return m_containerMap.value(id);
}

bool BitContainerManager::addContainer(QSharedPointer<BitContainer> container)
{
    m_containerMap.remove(container->id());
    m_containerMap.insert(container->id(), container);

    emit containerAdded(container);

    return true;
}

bool BitContainerManager::selectContainer(QSharedPointer<BitContainer> container)
{
    if (m_containerMap.contains(container->id())) {
        auto old = m_current;
        m_current = m_containerMap.value(container->id());
        emit currSelectionChanged(m_current, old);
        return true;
    }
    return false;
}

void BitContainerManager::deleteCurrentContainer()
{
    if (m_current.isNull()) {
        return;
    }
    auto old = m_current;
    m_containerMap.remove(m_current->id());
    m_current = QSharedPointer<BitContainer>();
    if (!m_containerMap.isEmpty()) {
        m_current = m_containerMap.values().first();
    }
    emit currSelectionChanged(m_current, old);
}

void BitContainerManager::deleteAllContainers()
{
    auto old = m_current;
    m_containerMap.clear();
    m_current = QSharedPointer<BitContainer>();
    emit currSelectionChanged(m_current, old);
}
