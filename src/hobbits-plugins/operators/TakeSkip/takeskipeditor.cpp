#include "takeskipeditor.h"
#include "ui_takeskipeditor.h"
#include <QMessageBox>

TakeSkipEditor::TakeSkipEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::TakeSkipEditor()),
    m_delegate(delegate),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_stateHelper->addLineEditStringParameter("take_skip_string", ui->le_takeSkip);
    m_stateHelper->addCheckBoxBoolParameter("interleaved", ui->cb_interleaved);
    m_stateHelper->addCheckBoxBoolParameter("frame_based", ui->cb_frameBased);
    m_stateHelper->addSpinBoxIntParameter("deinterleave_channels", ui->sb_deinterleave);

    connect(ui->btnInfo, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->le_takeSkip, SIGNAL(returnPressed()), this, SLOT(requestRun()));

    connect(ui->cb_interleaved, SIGNAL(toggled(bool)), this, SLOT(interleaveSelectionChanged()));
    connect(ui->cb_deinterleaved, SIGNAL(toggled(bool)), this, SLOT(interleaveSelectionChanged()));
    interleaveSelectionChanged();
}

TakeSkipEditor::~TakeSkipEditor()
{
    delete ui;
}

QString TakeSkipEditor::title()
{
    return "Configure Take Skip";
}

Parameters TakeSkipEditor::parameters()
{
    return m_stateHelper->getParametersFromUi();
}

bool TakeSkipEditor::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

void TakeSkipEditor::requestRun()
{
    emit accepted();
}

void TakeSkipEditor::showHelp()
{
    QMessageBox msg;
    msg.setText("Take Skip Commands");
    QString text = "t - take\ns - skip\nr - take in reverse\ni - invert\no - append a 1\nz - append a 0";
    text += "\n\n Uppercase letters can be used to indicate Bytes instead of bits (e.g. 'T10S4')";
    text += "\nThe character '*' can be used instead of a number to indicate 'all remaining bits'";
    text += "\nInterleave: apply the operation in a round-robin fashion between multiple input containers.";
    text += "\nFrame-based: apply the operation to each frame of the container independently";
    msg.setInformativeText(text);
    msg.setDefaultButton(QMessageBox::Cancel);
    msg.exec();
}

void TakeSkipEditor::interleaveSelectionChanged()
{
    ui->cb_deinterleaved->setEnabled(!ui->cb_interleaved->isChecked());
    ui->cb_interleaved->setEnabled(!ui->cb_deinterleaved->isChecked());
    ui->sb_deinterleave->setVisible(ui->cb_deinterleaved->isChecked());

    if (ui->cb_deinterleaved->isChecked()) {
        ui->sb_deinterleave->setRange(2, 100);
        ui->sb_deinterleave->setValue(2);
    }
    else {
        ui->sb_deinterleave->setRange(1, 1);
        ui->sb_deinterleave->setValue(1);
    }
}
