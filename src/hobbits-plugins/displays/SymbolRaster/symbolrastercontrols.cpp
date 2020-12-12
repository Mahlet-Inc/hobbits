#include "colordialogdelegate.h"
#include "symbolrastercontrols.h"
#include "ui_symbolrastercontrols.h"
#include <QJsonArray>
#include <QtMath>

SymbolRasterControls::SymbolRasterControls(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::SymbolRasterControls()),
    m_colorMapModel(new ColorMapModel()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);
    m_colorMapModel->setRemapLength(ui->sb_bitLength->value());

    ui->tv_mappings->setItemDelegateForColumn(1, new ColorDialogDelegate);
    ui->tv_mappings->setEditTriggers(
            QAbstractItemView::EditTrigger::SelectedClicked | QAbstractItemView::EditTrigger::DoubleClicked
            | QAbstractItemView::EditTrigger::AnyKeyPressed);
    ui->tv_mappings->setModel(m_colorMapModel);

    connect(ui->sb_bitLength, SIGNAL(valueChanged(int)), m_colorMapModel, SLOT(setRemapLength(int)));

    connect(m_colorMapModel,
            SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&,const QVector<int>&)),
            this,
            SIGNAL(changed()));
    connect(m_colorMapModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)), this, SIGNAL(changed()));
    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));

    m_stateHelper->addSliderIntParameter("scale", ui->hs_scale);
    m_stateHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
    m_stateHelper->addParameter("color_map", [this](QJsonValue value) {
        if (!value.isArray()) {
            return false;
        }
        QJsonArray values = value.toArray();
        if (values.size() < 1) {
            return false;
        }
        int bits = qFloor(log2(values.size()));
        if (bits < 1) {
            return false;
        }
        if (ui->sb_bitLength->value() != bits) {
            ui->sb_bitLength->setValue(bits);
        }
        QList<QPair<QString, QColor>> mappings;
        for (auto value : values) {
            if (!value.isObject()) {
                return false;
            }
            QJsonObject mapping = value.toObject();
            if (!mapping.contains("value") || !mapping.contains("color")) {
                return false;
            }
            mappings.append({mapping.value("value").toString(), QColor(mapping.value("color").toString())});
        }
        m_colorMapModel->setMappings(mappings);
        return true;
    }, [this]() {
        QJsonArray values;
        for (auto mapping : m_colorMapModel->getMappings()) {
            QJsonObject mappingObject;
            mappingObject.insert("value", mapping.first);
            mappingObject.insert("color", mapping.second.name());
            values.append(mappingObject);
        }
        return values;
    });
}

QString SymbolRasterControls::title()
{
    return "Configure Symbol Raster";
}

bool SymbolRasterControls::setParameters(QJsonObject parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

QJsonObject SymbolRasterControls::parameters()
{
    return m_stateHelper->getParametersFromUi();
}
