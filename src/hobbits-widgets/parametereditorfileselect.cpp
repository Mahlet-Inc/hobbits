#include "parametereditorfileselect.h"
#include "settingsmanager.h"
#include <QVBoxLayout>

ParameterEditorFileSelect::ParameterEditorFileSelect(
        QFileDialog::AcceptMode acceptMode,
        QString fileKey,
        QString editorTitle) :
    m_fileKey(fileKey),
    m_editorTitle(editorTitle)
{
    m_fileDialog = new QFileDialog(
                nullptr,
                tr(""),
                SettingsManager::getPrivateSetting(SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    m_fileDialog->setOption(QFileDialog::DontUseNativeDialog);
    m_fileDialog->setWindowFlags(Qt::Widget);
    m_fileDialog->setVisible(true);
    m_fileDialog->setAcceptMode(acceptMode);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_fileDialog);

    this->setLayout(layout);

    connect(m_fileDialog, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(m_fileDialog, SIGNAL(rejected()), this, SIGNAL(rejected()));
}

ParameterEditorFileSelect::~ParameterEditorFileSelect()
{
}

QString ParameterEditorFileSelect::title()
{
    return m_editorTitle;
}

bool ParameterEditorFileSelect::setParameters(QJsonObject parameters)
{
    if (parameters.contains(m_fileKey)) {
        m_fileDialog->selectFile(parameters.value(m_fileKey).toString());
        return true;
    }
    else {
        m_fileDialog->selectFile(QString());
        return false;
    }
}

QJsonObject ParameterEditorFileSelect::parameters()
{
    if (m_fileDialog->selectedFiles().isEmpty()) {
        return QJsonObject();
    }
    else {
        QJsonObject params;
        params.insert(m_fileKey, m_fileDialog->selectedFiles().at(0));
        return params;
    }
}

bool ParameterEditorFileSelect::isStandaloneDialog()
{
    return true;
}
