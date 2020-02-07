#include "colordialogdelegate.h"
#include "symbolrastercontrols.h"
#include "ui_symbolrastercontrols.h"

SymbolRasterControls::SymbolRasterControls() :
    ui(new Ui::SymbolRasterControls()),
    m_colorMapModel(new ColorMapModel())
{
    ui->setupUi(this);
    m_colorMapModel->setRemapLength(ui->sb_bitLength->value());

    ui->tv_mappings->setItemDelegateForColumn(1, new ColorDialogDelegate);
    ui->tv_mappings->setEditTriggers(
            QAbstractItemView::EditTrigger::SelectedClicked | QAbstractItemView::EditTrigger::DoubleClicked
            | QAbstractItemView::EditTrigger::AnyKeyPressed);
    ui->tv_mappings->setModel(m_colorMapModel);
    connect(ui->sb_bitLength, SIGNAL(valueChanged(int)), m_colorMapModel, SLOT(setRemapLength(int)));
    connect(
            m_colorMapModel,
            SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&,const QVector<int>&)),
            this,
            SLOT(updateColorMap()));
    connect(m_colorMapModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)), this, SLOT(updateColorMap()));

    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(scaleSet(int)));
}

void SymbolRasterControls::updateColorMap()
{
    emit colorMappingChanged(m_colorMapModel->getMappings());
}

void SymbolRasterControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}
