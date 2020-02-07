#include "pluginactionlineage.h"
#include "settingsmanager.h"
#include "templatefilehandler.h"
#include <QJsonArray>

TemplateFileHandler::TemplateFileHandler()
{

}

bool TemplateFileHandler::writeTemplate(
        QString fileName,
        QList<QSharedPointer<const PluginActionLineage>> lineages,
        QStringList *warnings)
{

    if (lineages.isEmpty()) {
        if (warnings) {
            warnings->append(QString("Cannot export an empty template").arg(fileName));
        }
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        if (warnings) {
            warnings->append(QString("Failed to open file '%1' for writing").arg(fileName));
        }
        return false;
    }

    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_TEMPLATE_PATH_KEY,
            QFileInfo(file).dir().path());

    QJsonDocument serialized;
    QJsonObject root;
    QJsonArray lineageArray;
    for (auto lineage : lineages) {
        lineageArray.append(lineage->serialize());
    }
    root.insert("lineages", lineageArray);
    serialized = QJsonDocument(root);

    file.write(serialized.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

void TemplateFileHandler::applyMultipleLineages(
        QUuid targetId,
        QSharedPointer<PluginActionLineage::TreeNode> lineageTree,
        QString name,
        QSharedPointer<BitContainerManager> bitManager,
        QSharedPointer<PluginActionManager> pluginActionManager)
{
    if (!lineageTree->lineage.isNull()) {
        // TODO: handle multiple outputs from a single op
        QUuid outputId = QUuid::createUuid();
        QMap<int, QUuid> outputIdOverride;
        outputIdOverride.insert(0, outputId);
        pluginActionManager->applyLineage(targetId, lineageTree->lineage, bitManager, name, outputIdOverride);

        targetId = outputId;
    }
    for (auto child : lineageTree->children) {
        applyMultipleLineages(targetId, child, name, bitManager, pluginActionManager);
    }
}

bool TemplateFileHandler::applyTemplateToContainer(
        QString fileName,
        QSharedPointer<BitContainer> bitContainer,
        QSharedPointer<BitContainerManager> bitManager,
        QSharedPointer<PluginActionManager> pluginActionManager,
        QStringList *warnings)
{
    if (bitContainer.isNull()) {
        if (warnings) {
            warnings->append(QString("Cannot apply a template to a null bit container").arg(fileName));
        }
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        if (warnings) {
            warnings->append(QString("Template file '%1' cannot be opened").arg(fileName));
        }
        return false;
    }
    QJsonDocument serialized = QJsonDocument::fromJson(file.read(5 * 1000 * 1000));
    QFileInfo fileInfo(file.fileName());
    QString templateName = fileInfo.fileName().section(".", 0, 0);

    SettingsManager::getInstance().setPrivateSetting(SettingsData::LAST_TEMPLATE_PATH_KEY, fileInfo.dir().path());

    QJsonObject lineageObject = serialized.object();
    if (lineageObject.contains("lineages") && lineageObject.value("lineages").isArray()) {
        QList<QSharedPointer<const PluginActionLineage>> lineages;
        for (auto lineage : lineageObject.value("lineages").toArray()) {
            if (!lineage.isObject()) {
                if (warnings) {
                    warnings->append(QString("Template file '%1' is not formatted correctly").arg(fileName));
                }
                return false;
            }
            auto actionLineage = PluginActionLineage::deserialize(lineage.toObject());
            if (actionLineage.isNull()) {
                if (warnings) {
                    warnings->append(QString("Template file '%1' is not formatted correctly").arg(fileName));
                }
                return false;
            }
            lineages.append(actionLineage);
        }
        applyMultipleLineages(
                bitContainer->getId(),
                PluginActionLineage::mergeIntoTree(lineages),
                templateName,
                bitManager,
                pluginActionManager);
    }
    else {
        auto actionLineage = PluginActionLineage::deserialize(lineageObject);
        if (actionLineage.isNull()) {
            if (warnings) {
                warnings->append(QString("Template file '%1' is not formatted correctly").arg(fileName));
            }
            return false;
        }
        pluginActionManager->applyLineage(bitContainer->getId(), actionLineage, bitManager, templateName);
    }
    return true;
}
