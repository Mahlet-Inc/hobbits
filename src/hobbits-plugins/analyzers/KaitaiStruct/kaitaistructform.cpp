#include "kaitaistructform.h"
#include "ui_kaitaistructform.h"
#include <QFileInfo>
#include <QMenu>
#include <QFileDialog>
#include "settingsmanager.h"
#include <QDirIterator>

KaitaiStructForm::KaitaiStructForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::KaitaiStructForm()),
    m_highlightNav(new HighlightNavigator()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addTextEditStringParameter("katai_struct_yaml", ui->te_ksy);

    m_paramHelper->addParameter("katai_struct_yaml",
    [this](QJsonValue value) {
        if (value.toString().isEmpty()) {
            return false;
        }
        ui->tabExecOptions->setCurrentIndex(1);
        ui->te_ksy->setPlainText(value.toString());
        return true;
    },
    [this]() {
        if (ui->tabExecOptions->currentIndex() != 1) {
            return QJsonValue(QJsonValue::Undefined);
        }
        return QJsonValue(ui->te_ksy->toPlainText());
    });

    m_paramHelper->addParameter("precompiled_py_file",
    [this](QJsonValue value) {
        if (value.toString().isEmpty()) {
            return false;
        }
        ui->tabExecOptions->setCurrentIndex(0);
        QFileInfo fileInfo(value.toString());
        m_selectedPrecompiledFile = value.toString();
        ui->pb_selectPrecompiled->setText("Parse as: "+ fileInfo.baseName());
        return true;
    },
    [this]() {
        if (ui->tabExecOptions->currentIndex() != 0) {
            return QJsonValue(QJsonValue::Undefined);
        }
        return QJsonValue(m_selectedPrecompiledFile);
    });

    m_loadKsyMenu = new QMenu();
    m_loadKsyMenu->addAction("Load File...", [this]() {
        QString fileName = QFileDialog::getOpenFileName(
                    nullptr,
                    tr("Select ksy File"),
                    SettingsManager::getPrivateSetting(KSY_PATH_KEY).toString(),
                    tr("Kaitai Struct File (*.ksy);;All Files (*)"));
        if (fileName.isEmpty()) {
            return;
        }
        QFile ksyFile(fileName);
        if (!ksyFile.open(QIODevice::ReadOnly)) {
            return;
        }
        SettingsManager::setPrivateSetting(KSY_PATH_KEY, QFileInfo(ksyFile).path());
        ui->te_ksy->setPlainText(ksyFile.readAll());
    });

    QDirIterator it(":/kaitaidata/ksy", QDir::Dirs | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        QDir category = it.next();
        QMenu* menu = m_loadKsyMenu->addMenu(category.dirName());
        for (QFileInfo ksyFileInfo :category.entryInfoList(QDir::Files)) {
            menu->addAction(ksyFileInfo.baseName(), [this, ksyFileInfo]() {
                QFile ksyFile(ksyFileInfo.filePath());
                if (!ksyFile.open(QIODevice::ReadOnly)) {
                    return;
                }
                ui->te_ksy->setPlainText(ksyFile.readAll());
            });
        }
    }

    m_loadKsyPyMenu = new QMenu();
    QDirIterator itPy(":/kaitaidata/ksy_py", QDir::Dirs | QDir::NoDotAndDotDot);
    while (itPy.hasNext()) {
        QDir category = itPy.next();
        QMenu* menu = m_loadKsyPyMenu->addMenu(category.dirName());
        for (QFileInfo ksyFileInfo :category.entryInfoList(QDir::Files)) {
            menu->addAction(ksyFileInfo.baseName(), [this, ksyFileInfo]() {
                m_selectedPrecompiledFile = ksyFileInfo.filePath();
                ui->pb_selectPrecompiled->setText("Parse as: "+ ksyFileInfo.baseName());
            });
        }
    }
    ui->pb_loadKsy->setMenu(m_loadKsyMenu);
    ui->pb_selectPrecompiled->setMenu(m_loadKsyPyMenu);

    connect(ui->tb_chooseKsc, SIGNAL(pressed()), this, SLOT(openKscPathDialog()));

    ui->le_ksc->setText(SettingsManager::getPrivateSetting(KAITAI_PATH_KEY).toString());

    m_highlightNav = new HighlightNavigator();
    ui->layout_nav->addWidget(m_highlightNav);
    ui->layout_nav->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_highlightNav->setHighlightCategory(KAITAI_STRUCT_CATEGORY);
    m_highlightNav->setShouldHighlightSelection(true);
}

KaitaiStructForm::~KaitaiStructForm()
{
    delete ui;
}

QString KaitaiStructForm::title()
{
    return "Configure Kaitai Struct Parser";
}

bool KaitaiStructForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

QJsonObject KaitaiStructForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

void KaitaiStructForm::giveDisplayHandle(QSharedPointer<DisplayHandle> handle)
{
    m_highlightNav->giveDisplayHandle(handle);
}

void KaitaiStructForm::openKscPathDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            tr("Select Kaitai Struct Compiler"),
            SettingsManager::getPrivateSetting(KAITAI_PATH_KEY).toString(),
            tr("KSC (ksc*, kaitai-struct-compiler*)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->le_ksc->setText(fileName);
    SettingsManager::setPrivateSetting(KAITAI_PATH_KEY, fileName);
}

void KaitaiStructForm::updateOutputText(QString text)
{
    ui->te_output->appendPlainText(text);
    ui->tabWidget->setCurrentIndex(1);
}

void KaitaiStructForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: figure out how to do the text output stuff

    m_highlightNav->setContainer(container);
    if (container.isNull()) {
        m_highlightNav->setTitle("");
    }
    else {
        QString resultLabel = container->info()->metadata(KAITAI_RESULT_LABEL).toString();
        if (resultLabel.size() > 28) {
            resultLabel.truncate(25);
            resultLabel += "...";
        }
        m_highlightNav->setTitle(resultLabel);
    }
}
