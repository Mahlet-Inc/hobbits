#ifndef REMAPMODEL_H
#define REMAPMODEL_H

#include <QAbstractTableModel>

class RemapModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RemapModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(
            const QModelIndex &index,
            const QVariant &value,
            int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QList<QPair<QString, QString>> getMappings();

public slots:
    void setRemapLength(int length);

private:
    void initializeMappings();

    int m_remapLength;
    QList<QPair<QString, QString>> m_mappings;

};

#endif // REMAPMODEL_H
