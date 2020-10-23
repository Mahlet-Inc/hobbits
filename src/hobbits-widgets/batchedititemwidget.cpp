#include "batchedititemwidget.h"
#include "ui_batchedititemwidget.h"
#include "parametereditordialog.h"

BatchEditItemWidget::BatchEditItemWidget(QString title, QSharedPointer<ParameterDelegate> delegate, QJsonObject parameters):
    ui(new Ui::BatchEditItemWidget),
    m_delegate(delegate)
{
    ui->setupUi(this);
    ui->lb_title->setText(title);

    if (m_delegate.isNull()) {
        ui->pb_config->setVisible(false);
        ui->pb_clear->setVisible(false);
        ui->te_config->setVisible(false);
    }
    else {
        setParameters(parameters);
    }
}

BatchEditItemWidget::~BatchEditItemWidget()
{
    delete ui;
}

QJsonObject BatchEditItemWidget::parameters() const
{
    return m_parameters;
}

QString BatchEditItemWidget::displayString() const
{
    if (m_delegate.isNull()) {
        return ui->lb_title->text();
    }

    return QString("%1: %2").arg(ui->lb_title->text()).arg(m_delegate->actionDescription(m_parameters));
}

void BatchEditItemWidget::setInputEnabled(bool enabled)
{
    ui->pb_inputs->setVisible(enabled);
}

void BatchEditItemWidget::setOutputEnabled(bool enabled)
{
    ui->pb_outputs->setVisible(enabled);
}

void BatchEditItemWidget::on_pb_inputs_clicked()
{
    emit inputPressed();
}

void BatchEditItemWidget::on_pb_outputs_clicked()
{
    emit outputPressed();
}

void BatchEditItemWidget::on_pb_config_clicked()
{
    QJsonObject parameters = ParameterEditorDialog::promptForParameters(m_delegate, m_parameters);
    if (parameters.isEmpty()) {
        return;
    }
    setParameters(parameters);
}

void BatchEditItemWidget::setParameters(QJsonObject parameters)
{
    m_parameters = parameters;
    QString description = m_delegate->actionDescription(m_parameters);
    ui->te_config->setPlainText(description);
    emit parametersChanged(m_parameters);
}

void BatchEditItemWidget::on_pb_clear_clicked()
{
    setParameters(QJsonObject());
}
