#include "colormapmodel.h"
#include <math.h>

#include "settingsmanager.h"
#include <QColor>
#include <QFont>
#include <QRegularExpression>

const QString COLOR[8] = {
    "symbol_raster_color_1",
    "symbol_raster_color_2",
    "symbol_raster_color_3",
    "symbol_raster_color_4",
    "symbol_raster_color_5",
    "symbol_raster_color_6",
    "symbol_raster_color_7",
    "symbol_raster_color_8"
};

ColorMapModel::ColorMapModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_remapLength(2)
{
    QList<QPair<QString, QString>> defaultColors = {
        {COLOR[0], "#d1bbd7"},
        {COLOR[1], "#882e72"},
        {COLOR[2], "#5289c7"},
        {COLOR[3], "#4eb265"},
        {COLOR[4], "#cae0ab"},
        {COLOR[5], "#f6c141"},
        {COLOR[6], "#e8601c"},
        {COLOR[7], "#ec050c"}
    };

    for (auto pair : defaultColors) {
        QVariant colorVar = SettingsManager::getPrivateSetting(pair.first);
        QColor c;
        if (colorVar.isNull() || !colorVar.canConvert<QColor>()) {
            c.setNamedColor(pair.second);
            SettingsManager::setPrivateSetting(pair.first, QVariant(c));
            m_defaultColors.append(c);
        }
        else {
            m_defaultColors.append(colorVar.value<QColor>());
        }
    }

    initializeMappings();
}

void ColorMapModel::initializeMappings()
{

    beginRemoveRows(QModelIndex(), 0, m_mappings.length() - 1);
    m_mappings.clear();
    endRemoveRows();

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
        m_mappings.append(QPair<QString, QColor>(mappingString, m_defaultColors.at(i % m_defaultColors.size())));
    }
    endInsertRows();
}

void ColorMapModel::setRemapLength(int length)
{
    this->m_remapLength = length;
    initializeMappings();
}

QList<QPair<QString, QColor>> ColorMapModel::getMappings()
{
    return m_mappings;
}

void ColorMapModel::setMappings(QList<QPair<QString, QColor> > mappings)
{
    beginRemoveRows(QModelIndex(), 0, m_mappings.length() - 1);
    m_mappings.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, mappings.length() - 1);
    m_mappings = mappings;
    endInsertRows();

    for (int i = 0; i < m_mappings.size(); i++) {
        if (m_mappings.at(i).second.isValid()) {
            SettingsManager::setPrivateSetting(COLOR[i % 8], m_mappings.at(i).second);
        }
    }
}

QVariant ColorMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return QVariant("Symbol");
        }
        else if (section == 1) {
            return QVariant("Color");
        }
    }

    return QVariant();
}

int ColorMapModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_mappings.size();
}

int ColorMapModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return 2;
}

QVariant ColorMapModel::data(const QModelIndex &index, int role) const
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
    else if (role == Qt::BackgroundRole) {
        return m_mappings.at(index.row()).second;
    }
    else if (role == Qt::ForegroundRole && index.column() == 1) {
        QColor color = m_mappings.at(index.row()).second;
        if (color.lightnessF() > 0.5) {
            return QColor(Qt::black);
        }
        else {
            return QColor(Qt::white);
        }
    }
    return QVariant();
}

bool ColorMapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && index.column() == 1) {
        if (!value.canConvert<QColor>()) {
            return false;
        }
        m_mappings.replace(
                index.row(),
                QPair<QString, QColor>(m_mappings.at(index.row()).first, value.value<QColor>()));

        SettingsManager::setPrivateSetting(COLOR[index.row() % 8], value);

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ColorMapModel::flags(const QModelIndex &index) const
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
