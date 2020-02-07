#ifndef TEMPLATEFILEHANDLER_H
#define TEMPLATEFILEHANDLER_H

#include "bitcontainer.h"
#include "bitcontainermanager.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include <QSharedPointer>
#include <QStringList>

class TemplateFileHandler
{
private:
    TemplateFileHandler();

public:
    static bool writeTemplate(
            QString fileName,
            QList<QSharedPointer<const PluginActionLineage>> lineages,
            QStringList *warnings = nullptr);

    static void applyMultipleLineages(
            QUuid targetId,
            QSharedPointer<PluginActionLineage::TreeNode> lineageTree,
            QString name,
            QSharedPointer<BitContainerManager> bitManager,
            QSharedPointer<PluginActionManager> pluginActionManager);

    static bool applyTemplateToContainer(
            QString fileName,
            QSharedPointer<BitContainer> bitContainer,
            QSharedPointer<BitContainerManager> bitManager,
            QSharedPointer<PluginActionManager> pluginActionManager,
            QStringList *warnings = nullptr);

};

#endif // TEMPLATEFILEHANDLER_H
