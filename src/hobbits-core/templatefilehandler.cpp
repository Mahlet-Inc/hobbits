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

void TemplateFileHandler::applyLineageTree(
        QUuid targetId,
        QList<QUuid> &additionalInputs,
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
        QList<QUuid> additionals;
        int requiredAdditionalInputs = lineageTree->lineage->additionalInputCount();
        while (requiredAdditionalInputs > 0) {
            additionals.append(additionalInputs.takeFirst());
            requiredAdditionalInputs--;
        }
        pluginActionManager->applyLineage(targetId, lineageTree->lineage, bitManager, name, outputIdOverride, additionals);

        targetId = outputId;
    }
    for (auto child : lineageTree->children) {
        applyLineageTree(targetId, additionalInputs, child, name, bitManager, pluginActionManager);
    }
}

QSharedPointer<PluginActionLineage::TreeNode> TemplateFileHandler::loadTemplate(QString fileName, QStringList *warnings)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        if (warnings) {
            warnings->append(QString("Template file '%1' cannot be opened").arg(fileName));
        }
        return QSharedPointer<PluginActionLineage::TreeNode>();
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
                return QSharedPointer<PluginActionLineage::TreeNode>();
            }
            auto actionLineage = PluginActionLineage::deserialize(lineage.toObject());
            if (actionLineage.isNull()) {
                if (warnings) {
                    warnings->append(QString("Template file '%1' is not formatted correctly").arg(fileName));
                }
                return QSharedPointer<PluginActionLineage::TreeNode>();
            }
            lineages.append(actionLineage);
        }
        return PluginActionLineage::mergeIntoTree(lineages);
    }
    else {
        auto actionLineage = PluginActionLineage::deserialize(lineageObject);
        if (actionLineage.isNull()) {
            if (warnings) {
                warnings->append(QString("Template file '%1' is not formatted correctly").arg(fileName));
            }
            return QSharedPointer<PluginActionLineage::TreeNode>();
        }
        return PluginActionLineage::mergeIntoTree({actionLineage});
    }
}
