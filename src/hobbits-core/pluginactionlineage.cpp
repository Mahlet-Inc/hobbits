#include "pluginactionlineage.h"
#include <QJsonArray>

PluginActionLineage::PluginActionLineage(QSharedPointer<const PluginAction> pluginAction) :
    m_pluginAction(pluginAction),
    m_outputPosition(0)
{
}

QSharedPointer<PluginActionLineage> PluginActionLineage::create(QSharedPointer<const PluginAction> pluginAction)
{
    return QSharedPointer<PluginActionLineage>(new PluginActionLineage(pluginAction));
}

QSharedPointer<const PluginActionLineage> PluginActionLineage::fromLineage(
        QSharedPointer<const PluginActionLineage> lineage,
        int offset,
        int length)
{
    QList<QSharedPointer<const PluginActionLineage>> lineageParts = lineage->getLineage();
    int lastIndex = lineageParts.size();
    if (length >= 0) {
        lastIndex = offset + length;
    }
    QSharedPointer<const PluginActionLineage> lastOne;
    for ( ; offset < lineageParts.size() && offset < lastIndex; offset++) {
        QSharedPointer<PluginActionLineage> l = PluginActionLineage::create(lineageParts.at(offset)->getPluginAction());
        if (!lastOne.isNull()) {
            l->setParent(lastOne);
            l->setOutputPos(lineageParts.at(offset)->getOutputPosition());
        }
        lastOne = l;
    }

    return lastOne;
}

QSharedPointer<PluginActionLineage::TreeNode> PluginActionLineage::mergeIntoTree(
        QList<QSharedPointer<const PluginActionLineage>> branchingLineages)
{

    QSharedPointer<PluginActionLineage::TreeNode> node = QSharedPointer<PluginActionLineage::TreeNode>(
            new PluginActionLineage::TreeNode());

    if (branchingLineages.size() == 1) {
        node->lineage = branchingLineages.at(0);
        return node;
    }

    QMultiHash<PluginAction, QSharedPointer<const PluginActionLineage>> childBranches;
    for (int i = 0; i < branchingLineages.size(); i++) {
        auto singleLineage = branchingLineages.at(i)->getLineage();
        auto firstAction = *singleLineage.at(0)->getPluginAction().data();
        childBranches.insert(firstAction, branchingLineages.at(i));
    }

    if (childBranches.uniqueKeys().size() == 1) {
        // find the depth of the common lineage
        int sharedLength = 0;
        bool shared = true;
        QSet<PluginAction> actions;
        while (shared) {
            sharedLength++;
            actions.clear();
            for (auto lineage : branchingLineages) {
                auto singleLineage = lineage->getLineage();
                if (singleLineage.size() <= sharedLength) {
                    shared = false;
                    break;
                }
                actions.insert(*singleLineage.at(sharedLength)->getPluginAction().data());
                if (actions.size() > 1) {
                    shared = false;
                    break;
                }
            }
        }

        node->lineage = PluginActionLineage::fromLineage(branchingLineages.at(0), 0, sharedLength);
        QList<QSharedPointer<const PluginActionLineage>> childList;
        for (auto lineage : branchingLineages) {
            childList.append(PluginActionLineage::fromLineage(lineage, sharedLength));
        }
        node->children.append(PluginActionLineage::mergeIntoTree(childList));
    }
    else {
        for (PluginAction key : childBranches.uniqueKeys()) {
            node->children.append(PluginActionLineage::mergeIntoTree(childBranches.values(key)));
        }
    }

    return node;
}

QSharedPointer<PluginActionLineage> PluginActionLineage::setParent(QSharedPointer<const PluginActionLineage> parent)
{
    m_parent = parent;
    return sharedFromThis();
}


QSharedPointer<PluginActionLineage> PluginActionLineage::addAdditionalInput(
        QSharedPointer<const PluginActionLineage> inputLineage)
{
    m_additionalInputs.append(inputLineage);
    return sharedFromThis();
}

QSharedPointer<PluginActionLineage> PluginActionLineage::setOutputPos(int outputPosition)
{
    m_outputPosition = outputPosition;
    return sharedFromThis();
}

QSharedPointer<const PluginAction> PluginActionLineage::getPluginAction() const
{
    return m_pluginAction;
}

int PluginActionLineage::getOutputPosition() const
{
    return m_outputPosition;
}

QList<QSharedPointer<const PluginActionLineage>> PluginActionLineage::getAdditionalInputs() const
{
    return m_additionalInputs;
}

QJsonObject PluginActionLineage::serialize() const
{
    QJsonObject obj;
    QJsonArray actions;
    for (auto action : getLineage()) {
        QJsonObject actionObject;
        actionObject.insert("action", action->m_pluginAction->serialize());
        actionObject.insert("outputPosition", action->m_outputPosition);
        QJsonArray additionalInputs;
        for (auto input : m_additionalInputs) {
            if (input.isNull()) {
                additionalInputs.append(QJsonObject());
            }
            else {
                additionalInputs.append(input->serialize());
            }
        }
        actionObject.insert("additionalInputs", additionalInputs);
        actions.append(actionObject);
    }
    obj.insert("actions", actions);
    return obj;
}

QSharedPointer<PluginActionLineage> PluginActionLineage::deserialize(QJsonObject data)
{
    QSharedPointer<PluginActionLineage> nullLineage;

    if (!(data.contains("actions")
          && data.value("actions").isArray())) {
        return nullLineage;
    }

    QSharedPointer<PluginActionLineage> lineage;
    for (auto action : data.value("actions").toArray()) {
        if (!action.isObject()) {
            return nullLineage;
        }
        QJsonObject actionObject = action.toObject();
        if (!(actionObject.contains("action")
              && actionObject.value("action").isObject()
              && actionObject.contains("outputPosition")
              && actionObject.value("outputPosition").isDouble())) {
            return nullLineage;
        }
        QSharedPointer<PluginAction> pluginAction = PluginAction::deserialize(actionObject.value("action").toObject());
        if (pluginAction.isNull()) {
            return nullLineage;
        }
        int outputPosition = int(actionObject.value("outputPosition").toDouble());

        lineage = PluginActionLineage::create(pluginAction)->setParent(lineage)->setOutputPos(outputPosition);

        if (actionObject.contains("additionalInputs")
                && actionObject.value("additionalInputs").isArray()) {
            for (auto additional : actionObject.value("additionalInputs").toArray()) {
                if (!additional.isObject()) {
                    return nullLineage;
                }
                QJsonObject additionalObject = additional.toObject();
                if (additionalObject.size() == 0) {
                    lineage->addAdditionalInput(QSharedPointer<PluginActionLineage>());
                }
                else {
                    lineage->addAdditionalInput(PluginActionLineage::deserialize(additionalObject));
                }
            }
        }
    }

    return lineage;
}

QList<QSharedPointer<const PluginActionLineage>> PluginActionLineage::getLineage() const
{
    QList<QSharedPointer<const PluginActionLineage>> lineage;
    if (!m_parent.isNull()) {
        lineage = m_parent->getLineage();
    }
    lineage.append(this->sharedFromThis());
    return lineage;
}

int PluginActionLineage::additionalInputCount() const
{
    int count = 0;
    for (auto lineage: getLineage()) {
        count += lineage->getAdditionalInputs().size();
    }
    return count;
}
