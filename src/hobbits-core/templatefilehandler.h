#ifndef TEMPLATEFILEHANDLER_H
#define TEMPLATEFILEHANDLER_H

#include "bitcontainer.h"
#include "bitcontainermanager.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include <QSharedPointer>
#include <QStringList>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT TemplateFileHandler
{
private:
    TemplateFileHandler();

public:
    static bool writeTemplate(
            QString fileName,
            QList<QSharedPointer<const PluginActionLineage>> lineages,
            QStringList *warnings = nullptr);

    static void applyLineageTree(
            QUuid targetId,
            QList<QUuid> &additionalInputs,
            QSharedPointer<PluginActionLineage::TreeNode> lineageTree,
            QString name,
            QSharedPointer<BitContainerManager> bitManager,
            QSharedPointer<PluginActionManager> pluginActionManager);

    static QSharedPointer<PluginActionLineage::TreeNode> loadTemplate(
            QString fileName,
            QStringList *warnings = nullptr);

};

#endif // TEMPLATEFILEHANDLER_H
