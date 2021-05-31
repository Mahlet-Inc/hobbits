#include "parametereditorfileselect.h"
#include "settingsmanager.h"
#include "widgetssettings.h"
#include <QVBoxLayout>

ParameterEditorFileSelect::ParameterEditorFileSelect(
        QFileDialog::AcceptMode acceptMode,
        QString fileKey,
        QString editorTitle,
        QString stateKey) :
    m_fileKey(fileKey),
    m_editorTitle(editorTitle)
{
    m_fileDialog = new QFileDialog(
                nullptr,
                tr(""));
    m_fileDialog->setOption(QFileDialog::DontUseNativeDialog);
    m_fileDialog->setWindowFlags(Qt::Widget);
    m_fileDialog->setVisible(true);
    m_fileDialog->setAcceptMode(acceptMode);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    if (stateKey.isNull()) {
        stateKey = SettingsManager::LAST_IMPORT_EXPORT_KEY;
    }

    QString dirKey = WidgetsSettings::dialogDirKey(stateKey);
    QString sizeKey = WidgetsSettings::dialogRectKey(stateKey);

    QVariant lastDir = SettingsManager::getPrivateSetting(dirKey);
    if (lastDir.isValid() && lastDir.canConvert<QString>()) {
        m_fileDialog->setDirectory(lastDir.toString());
    }

    QVariant lastSize = SettingsManager::getPrivateSetting(sizeKey);
    if (lastSize.isValid() && lastSize.canConvert<QSize>()) {
        m_fileDialog->resize(lastSize.toSize());
    }

    resize(m_fileDialog->size());

    layout->addWidget(m_fileDialog);

    this->setLayout(layout);

    connect(m_fileDialog, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(m_fileDialog, SIGNAL(rejected()), this, SIGNAL(rejected()));

    connect(m_fileDialog, &QDialog::finished, [this, dirKey, sizeKey](int result) {
        auto path = m_fileDialog->directory().path().toStdString();
        auto saveBytes = this->m_fileDialog->saveState().toStdString();
        
        SettingsManager::setPrivateSetting(dirKey, m_fileDialog->directory().path());
        SettingsManager::setPrivateSetting(sizeKey, m_fileDialog->size());
    });
}

ParameterEditorFileSelect::~ParameterEditorFileSelect()
{

}

QString ParameterEditorFileSelect::title()
{
    return m_editorTitle;
}

bool ParameterEditorFileSelect::setParameters(const Parameters &parameters)
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

Parameters ParameterEditorFileSelect::parameters()
{
    if (m_fileDialog->selectedFiles().isEmpty()) {
        return Parameters::nullParameters();
    }
    else {
        Parameters params;
        params.insert(m_fileKey, m_fileDialog->selectedFiles().at(0));
        return params;
    }
}

bool ParameterEditorFileSelect::isStandaloneDialog()
{
    return true;
}
