#include "remapmodel.h"
#include <math.h>

#include <QFont>
#include <QRegularExpression>

RemapModel::RemapModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_remapLength(2)
{
    initializeMappings();
}

void RemapModel::initializeMappings()
{
    if (m_mappings.length() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_mappings.length() - 1);
        m_mappings.clear();
        endRemoveRows();
    }

    int symbolsToMap = (1 << m_remapLength);

    beginInsertRows(QModelIndex(), 0, symbolsToMap - 1);
    for (int i = 0; i < symbolsToMap; i++) {
        QString mappingString = "";
        for (int bit = m_remapLength - 1; bit >= 0; bit--) {
            if (i & (1 << bit)) {
                mappingString += "1";
            }
            else {
                mappingString += "0";
            }
        }
        m_mappings.append(QPair<QString, QString>(mappingString, mappingString));
    }
    endInsertRows();
}

void RemapModel::setRemapLength(int length)
{
    this->m_remapLength = length;
    initializeMappings();
}

QList<QPair<QString, QString>> RemapModel::getMappings()
{
    return m_mappings;
}

QVariant RemapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return QVariant("Old");
        }
        else if (section == 1) {
            return QVariant("New");
        }
    }

    return QVariant();
}

int RemapModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_mappings.size();
}

int RemapModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return 2;
}

QVariant RemapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return m_mappings.at(index.row()).first;
        }
        else if (index.column() == 1) {
            return m_mappings.at(index.row()).second;
        }
    }
    else if (role == Qt::FontRole) {
        return QFont("Roboto Mono");
    }
    return QVariant();
}

bool RemapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && index.column() == 1) {

        QString mapValue = value.toString();
        if (mapValue.length() != m_mappings.at(index.row()).first.length()) {
            return false;
        }

        QRegularExpression binMatcher("^[0-1]+$");
        QRegularExpressionMatch match = binMatcher.match(mapValue);
        if (!match.hasMatch()) {
            return false;
        }

        m_mappings.replace(
                index.row(),
                QPair<QString, QString>(m_mappings.at(index.row()).first, mapValue));

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags RemapModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else {
        return Qt::NoItemFlags;
    }
}
