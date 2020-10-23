#include "parametereditordialog.h"
#include "ui_parametereditordialog.h"
#include "abstractparametereditor.h"

ParameterEditorDialog::ParameterEditorDialog(
        QSharedPointer<ParameterDelegate> delegate,
        QJsonObject parameters,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterEditorDialog)
{
    ui->setupUi(this);

    m_editor = delegate->createEditor();
    if (m_editor == nullptr) {
        return;
    }
    if (!parameters.isEmpty()) {
        m_editor->setParameters(parameters);
    }

    setWindowTitle(m_editor->title());

    ui->mainLayout->insertWidget(0, m_editor);

    if (m_editor->isStandaloneDialog()) {
        ui->layout_okCancel->setEnabled(false);
        ui->bb_okCancel->setVisible(false);
    }

    connect(m_editor, &AbstractParameterEditor::accepted, this, &QDialog::accept);
    connect(m_editor, &AbstractParameterEditor::rejected, this, &QDialog::reject);
}

ParameterEditorDialog::~ParameterEditorDialog()
{
    delete ui;
}

void ParameterEditorDialog::setParameters(QJsonObject parameters)
{
    if (!m_editor) {
        return;
    }
    m_editor->setParameters(parameters);
}

QJsonObject ParameterEditorDialog::parameters()
{
    if (!m_editor) {
        return QJsonObject();
    }
    return m_editor->parameters();
}

QJsonObject ParameterEditorDialog::promptForParameters(QSharedPointer<ParameterDelegate> delegate, QJsonObject parameters)
{
    QScopedPointer<ParameterEditorDialog> dialog(new ParameterEditorDialog(delegate, parameters));

    if (!dialog->exec()) {
        return QJsonObject();
    }

    return dialog->parameters();
}
