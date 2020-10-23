#ifndef PLUGINTREEMODEL_H
#define PLUGINTREEMODEL_H

#include <QAbstractItemModel>
#include "hobbitspluginmanager.h"
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT PluginTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PluginTreeModel(QSharedPointer<HobbitsPluginManager> pluginManager, QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(
            int section,
            Qt::Orientation orientation,
            int role = Qt::DisplayRole) const override;

    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;

private:
    QList<QSharedPointer<OperatorInterface>> m_operators;
    QList<QSharedPointer<AnalyzerInterface>> m_analyzers;
    QList<QSharedPointer<ImporterExporterInterface>> m_importers;
    QList<QSharedPointer<ImporterExporterInterface>> m_exporters;

    QList<QPair<QString, int>> m_rowConfig;
};

#endif // PLUGINTREEMODEL_H
