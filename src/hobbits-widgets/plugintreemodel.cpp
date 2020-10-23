#include "plugintreemodel.h"
#include <QFont>
#include <QMimeData>

#define ROOT_ID 1024u
#define IMPORT_LABEL "Importers"
#define EXPORT_LABEL "Exporters"
#define OP_LABEL "Operators"
#define ANALYZE_LABEL "Analyzers"
#define BATCH_INPUT_LABEL "Batch Input"

PluginTreeModel::PluginTreeModel(QSharedPointer<HobbitsPluginManager> pluginManager, QObject *parent) :
    QAbstractItemModel(parent)
{
    m_operators = pluginManager->operators();
    m_analyzers = pluginManager->analyzers();
    for (auto importerExporter : pluginManager->importerExporters()) {
        if (importerExporter->canImport()) {
            m_importers.append(importerExporter);
        }
        if (importerExporter->canExport()) {
            m_exporters.append(importerExporter);
        }
    }

    m_rowConfig.append({IMPORT_LABEL, m_importers.size()});
    m_rowConfig.append({EXPORT_LABEL, m_exporters.size()});
    m_rowConfig.append({ANALYZE_LABEL, m_analyzers.size()});
    m_rowConfig.append({OP_LABEL, m_operators.size()});
    m_rowConfig.append({BATCH_INPUT_LABEL, 1});
}

QModelIndex PluginTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (parent.isValid()) {
        return createIndex(row, column, quintptr(parent.row()));
    }
    else {
        return createIndex(row, column, ROOT_ID);
    }
}

QModelIndex PluginTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    if (index.internalId() == ROOT_ID) {
        return QModelIndex();
    }
    else {
        return this->index(int(index.internalId()), 0);
    }
}

int PluginTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_rowConfig.size();
    }
    else if (!parent.parent().isValid()) {
        return m_rowConfig.at(parent.row()).second;
    }

    return 0;
}

int PluginTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant PluginTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (!index.parent().isValid()) {
        if (role == Qt::DisplayRole) {
            return QVariant(m_rowConfig.at(index.row()).first);
        }
        else if (role == Qt::FontRole) {
            QFont font;
            font.setBold(true);
            return QVariant(font);
        }
        return QVariant();
    }

    if (m_rowConfig.at(index.parent().row()).first == IMPORT_LABEL) {
        if (role == Qt::DisplayRole) {
            return QVariant(m_importers.at(index.row())->name());
        }
        else if (role == Qt::UserRole) {
            return QVariant(m_importers.at(index.row())->name());
        }
    }
    else if (m_rowConfig.at(index.parent().row()).first == EXPORT_LABEL) {
        if (role == Qt::DisplayRole) {
            return QVariant(m_exporters.at(index.row())->name());
        }
        else if (role == Qt::UserRole) {
            return QVariant(m_exporters.at(index.row())->name());
        }
    }
    else if (m_rowConfig.at(index.parent().row()).first == OP_LABEL) {
        if (role == Qt::DisplayRole) {
            return QVariant(m_operators.at(index.row())->name());
        }
        else if (role == Qt::UserRole) {
            return QVariant(m_operators.at(index.row())->name());
        }
    }
    else if (m_rowConfig.at(index.parent().row()).first == ANALYZE_LABEL) {
        if (role == Qt::DisplayRole) {
            return QVariant(m_analyzers.at(index.row())->name());
        }
        else if (role == Qt::UserRole) {
            return QVariant(m_analyzers.at(index.row())->name());
        }
    }
    else if (m_rowConfig.at(index.parent().row()).first == BATCH_INPUT_LABEL) {
        if (role == Qt::DisplayRole) {
            return QVariant("Anonymous Batch Input");
        }
        else if (role == Qt::UserRole) {
            return QVariant("Batch Input");
        }
    }

    return QVariant();
}

Qt::ItemFlags PluginTreeModel::flags(const QModelIndex &index) const
{

    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    unsigned int f = QAbstractItemModel::flags(index);

    if (index.parent().isValid()) {
        f |= Qt::ItemIsDragEnabled;
    }
    else {
        f ^= Qt::ItemIsSelectable;
    }

    return static_cast<Qt::ItemFlags>(f);
}

QVariant PluginTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

QMimeData *PluginTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndex index = indexes.first();
    if (!index.isValid() || !index.parent().isValid()) {
        return nullptr;
    }

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(index.data(Qt::DisplayRole).toString());

    if (m_rowConfig.at(index.parent().row()).first == IMPORT_LABEL) {
        mimeData->setData("text/hobbits/plugin/importer", index.data(Qt::UserRole).toByteArray());
    }
    else if (m_rowConfig.at(index.parent().row()).first == EXPORT_LABEL) {
        mimeData->setData("text/hobbits/plugin/exporter", index.data(Qt::UserRole).toByteArray());
    }
    else if (m_rowConfig.at(index.parent().row()).first == ANALYZE_LABEL) {
        mimeData->setData("text/hobbits/plugin/analyzer", index.data(Qt::UserRole).toByteArray());
    }
    else if (m_rowConfig.at(index.parent().row()).first == OP_LABEL) {
        mimeData->setData("text/hobbits/plugin/operator", index.data(Qt::UserRole).toByteArray());
    }
    else if (m_rowConfig.at(index.parent().row()).first == BATCH_INPUT_LABEL) {
        mimeData->setData("text/hobbits/batchinput", index.data(Qt::UserRole).toByteArray());
    }

    return mimeData;
}

QStringList PluginTreeModel::mimeTypes() const
{
    QStringList types;
    types << "text/hobbits/plugin/operator";
    types << "text/hobbits/plugin/analyzer";
    types << "text/hobbits/plugin/importer";
    types << "text/hobbits/plugin/exporter";
    return types;
}
