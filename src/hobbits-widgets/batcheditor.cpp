#include "batcheditor.h"
#include "ui_batcheditor.h"
#include "plugintreemodel.h"
#include "widgetssettings.h"
#include <QFileDialog>
#include <QMessageBox>

BatchEditor::BatchEditor(QSharedPointer<HobbitsPluginManager> pluginManager, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::BatchEditor),
    m_pluginManager(pluginManager)
{
    ui->setupUi(this);

    ui->menu_View->addAction(ui->dock_plugins->toggleViewAction());

    m_editScene = new BatchEditScene(m_pluginManager);
    ui->gv_flowGraph->setScene(m_editScene);
    ui->gv_flowGraph->setAcceptDrops(true);

    auto pluginModel = new PluginTreeModel(pluginManager);
    ui->tv_plugins->setModel(pluginModel);

    setWindowTitle("Batch Editor");
}

BatchEditor::~BatchEditor()
{
    delete ui;
}

void BatchEditor::setBatch(QSharedPointer<PluginActionBatch> batch)
{
    m_editScene->setBatch(batch);
}

void BatchEditor::on_action_saveBatchAs_triggered()
{
    auto batch = m_editScene->getBatch();

    QString fileName = WidgetsSettings::getFile(
        this,
        tr("Save Batch As"),
        QDir::homePath(),
        tr("Hobbits Batch Files (*.hbat)"),
        QFileDialog::AcceptSave,
        QFileDialog::AnyFile,
        SettingsManager::LAST_BATCH_KEY
    );

    if (fileName.isEmpty()) {
        return;
    }
    if (!fileName.endsWith(".hbat")) {
        fileName += ".hbat";
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Cannot Save Batch", QString("Could not open file '%1' for writing").arg(fileName));
        return;
    }

    QJsonDocument json(batch->serialize());
    file.write(json.toJson());
}

void BatchEditor::on_action_openBatch_triggered()
{
    QString fileName = WidgetsSettings::getFile(
        this,
        tr("Open Batch"),
        QDir::homePath(),
        tr("Hobbits Batch Files (*.hbat)"),
        QFileDialog::AcceptOpen,
        QFileDialog::ExistingFile,
        SettingsManager::LAST_BATCH_KEY
    );

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Cannot Open Batch", QString("Could not open file '%1'").arg(fileName));
        return;
    }

    auto batch = PluginActionBatch::deserialize(QJsonDocument::fromJson(file.readAll()).object());

    if (batch.isNull()) {
        QMessageBox::warning(this,
                             "Cannot Open Batch",
                             QString("Format of hobbits batch file could not be read '%1'").arg(fileName));
        return;
    }

    m_editScene->setBatch(batch);
}

void BatchEditor::on_action_createNewBatch_triggered()
{
    auto reply = QMessageBox::question(this, "Confirm Starting New Batch",
                                       "Are you sure you want to start editing a new batch? Unsaved changes will be lost.");
    if (reply != QMessageBox::Yes) {
        return;
    }
    m_editScene->resetBatch();
}
