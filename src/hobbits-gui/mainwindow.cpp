#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QPluginLoader>
#include <QQueue>
#include <QSettings>
#include <QWhatsThis>

#include "displaywidget.h"
#include "containerselectiondialog.h"
#include "hobbitscoreconfig.h"
#include "hobbitsguiconfig.h"
#include "hobbitspythonconfig.h"
#include "pluginactionlineage.h"
#include "preferencesdialog.h"
#include "settingsmanager.h"
#include "batchcreationdialog.h"
#include "abstractparametereditor.h"
#include "parametereditordialog.h"
#include "batcheditor.h"
#include "widgetssettings.h"

#include "pythonpluginconfig.h"
#include "simpleparametereditor.h"

const int MAINWINDOW_STATE_VERSION = 1;

static QString BATCH_EDITOR_SIZE_KEY = "batch_editor_size";
static QString BATCH_EDITOR_POSITION_KEY = "batch_editor_pos";

MainWindow::MainWindow(QString extraPluginPath, QString configFilePath, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_extraPluginPath(extraPluginPath),
    m_bitContainerManager(QSharedPointer<BitContainerManagerUi>(new BitContainerManagerUi())),
    m_pluginManager(QSharedPointer<HobbitsPluginManager>(new HobbitsPluginManager())),
    m_pluginActionManager(new PluginActionManager(m_pluginManager)),
    m_previewScroll(new PreviewScrollBar())
{
    m_pluginActionManager->setContainerManager(m_bitContainerManager);
    ui->setupUi(this);

    if (!configFilePath.isEmpty()) {
        SettingsManager::setConfigFilePath(configFilePath);
    }
    resize(SettingsManager::getPrivateSetting(SettingsManager::WINDOW_SIZE_KEY).toSize());
    move(SettingsManager::getPrivateSetting(SettingsManager::WINDOW_POSITION_KEY).toPoint());
    restoreState(SettingsManager::getPrivateSetting(SettingsManager::WINDOW_STATE_KEY).toByteArray(), MAINWINDOW_STATE_VERSION);

    // Populate View Menu
    ui->menu_View->addAction(ui->dock_bitContainerSelect->toggleViewAction());
    ui->menu_View->addAction(ui->dock_operatorPlugins->toggleViewAction());
    ui->menu_View->addAction(ui->dock_findBits->toggleViewAction());

    ui->dock_bitContainerSelect->setContentsMargins(0, 0, 0, 0);
    ui->dock_operatorPlugins->setContentsMargins(0, 0, 0, 0);
    ui->dock_findBits->setContentsMargins(0, 0, 0, 0);
    ui->dock_bitContainerSelect->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B));
    ui->dock_operatorPlugins->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    ui->dock_findBits->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    // More menu initialization
    populateRecentBatchesMenu();
    populatePresetBatchesMenu();

    // Configure Bit Container View
    ui->tv_bitContainers->setModel(m_bitContainerManager->getTreeModel().data());

    ui->tv_bitContainers->setSelectionModel(m_bitContainerManager->getCurrSelectionModel().data());

    connect(
            m_bitContainerManager.data(),
            &BitContainerManager::currSelectionChanged,
            this,
            &MainWindow::activateBitContainer);

    connect(
            ui->tb_removeBitContainer,
            &QPushButton::pressed,
            this,
            &MainWindow::deleteCurrentBitcontainer);

    QMenu *containersMenu = new QMenu(this);
    containersMenu->addAction(tr("Remove Current Bit Container..."), [this]() {
        this->deleteCurrentBitcontainer();
    });
    containersMenu->addAction(tr("Remove All Bit Containers..."), [this]() {
        this->deleteAllBitContainers();
    });
    ui->tb_containersMenu->setMenu(containersMenu);
    ui->tb_containersMenu->setPopupMode(QToolButton::InstantPopup);

    // Configure Plugin Action Management
    connect(
            ui->operatorTabs,
            &QTabWidget::currentChanged,
            this,
            [=](){ checkOperatorInput(); } );

    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::analyzerStarted,
            this,
            &MainWindow::pluginActionStarted);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::analyzerProgress,
            this,
            &MainWindow::pluginActionProgress);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::analyzerFinished,
            this,
            &MainWindow::pluginActionFinished);

    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::operatorStarted,
            this,
            &MainWindow::pluginActionStarted);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::operatorProgress,
            this,
            &MainWindow::pluginActionProgress);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::operatorFinished,
            this,
            &MainWindow::pluginActionFinished);

    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::importerStarted,
            this,
            &MainWindow::pluginActionStarted);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::importerProgress,
            this,
            &MainWindow::pluginActionProgress);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::importerFinished,
            this,
            &MainWindow::pluginActionFinished);

    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::exporterStarted,
            this,
            &MainWindow::pluginActionStarted);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::exporterProgress,
            this,
            &MainWindow::pluginActionProgress);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::exporterFinished,
            this,
            &MainWindow::pluginActionFinished);

    connect(
            m_pluginActionManager.data(),
            SIGNAL(reportError(QString)),
            this,
            SLOT(warningMessage(QString)));

    // Configure display handle and plugin callback
    ui->displayScrollLayout->addWidget(m_previewScroll);
    m_displayHandle = QSharedPointer<DisplayHandle>(new DisplayHandle(m_bitContainerManager));
    m_displayHandle->setBitOffsetControl(ui->displayHScroll);
    m_displayHandle->setFrameOffsetControl(ui->displayVScroll);
    connect(
            m_displayHandle.data(),
            &DisplayHandle::newStatus,
            this,
            &MainWindow::setStatus);

    m_previewScroll->setBitContainerManager(m_bitContainerManager);
    m_previewScroll->setDisplayHandle(m_displayHandle);

    // populate top level "What's This" info
    ui->action_Whats_This->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_W));
    ui->dock_bitContainerSelect->setWhatsThis("<b>Bit Containers Dock:</b> This is where you can navigate between the various Bit Containers that you are working with and see their lineage.");

    // load and initialize plugins
    loadPlugins();
    initializeImporterExporters();
    initializeDisplays();

    // Import menu initialization
    populateRecentImportsMenu();
    populateRecentExportsMenu();

    // create an initial state
    checkOperatorInput();
    activateBitContainer(currContainer(), QSharedPointer<BitContainer>());

    m_batchEditor = new BatchEditor(m_pluginManager, this);
    if (SettingsManager::getPrivateSetting(BATCH_EDITOR_SIZE_KEY).isValid()) {
        m_batchEditor->resize(SettingsManager::getPrivateSetting(BATCH_EDITOR_SIZE_KEY).toSize());
    }
    if (SettingsManager::getPrivateSetting(BATCH_EDITOR_POSITION_KEY).isValid()) {
        m_batchEditor->move(SettingsManager::getPrivateSetting(BATCH_EDITOR_POSITION_KEY).toPoint());
    }
}

MainWindow::~MainWindow()
{
    // release the offset controls from DisplayHandle before deleting UI
    m_displayHandle->deactivate();

    delete ui;
}

const QString SPLIT_DISPLAY_CONFIG_KEY = "split_display_full_config";
void MainWindow::closeEvent(QCloseEvent *event)
{
    SettingsManager::setPrivateSetting(SettingsManager::WINDOW_SIZE_KEY, size());
    SettingsManager::setPrivateSetting(SettingsManager::WINDOW_POSITION_KEY, pos());
    SettingsManager::setPrivateSetting(SettingsManager::WINDOW_STATE_KEY, saveState(MAINWINDOW_STATE_VERSION));

    SettingsManager::setPrivateSetting(BATCH_EDITOR_SIZE_KEY, m_batchEditor->size());
    SettingsManager::setPrivateSetting(BATCH_EDITOR_POSITION_KEY, m_batchEditor->pos());

    QByteArray splitDisplayConfig = m_rootDisplay->saveState();
    SettingsManager::setPrivateSetting(SPLIT_DISPLAY_CONFIG_KEY, splitDisplayConfig);

    event->accept();
}

void MainWindow::initializeDisplays()
{
    m_rootDisplay = new DisplaySplitter(m_pluginManager, m_displayHandle);
    ui->displayTabsLayout->addWidget(m_rootDisplay);
    m_rootDisplay->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // Add display splits if they were saved in the config
    QVariant savedConfig = SettingsManager::getPrivateSetting(SPLIT_DISPLAY_CONFIG_KEY);
    if (!savedConfig.isNull() && savedConfig.canConvert<QByteArray>()) {
        m_rootDisplay->restoreState(savedConfig.toByteArray());
    }

    ui->menu_View->addSeparator();
    auto tabShowToggle = ui->menu_View->addAction("Display Tabs", [this](bool show) {
        m_rootDisplay->setShowViewSelect(show);
    }, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));
    tabShowToggle->setCheckable(true);
    tabShowToggle->setChecked(true);

    ui->menu_View->addAction("Collapse All Displays", [this]() {
        m_rootDisplay->unSplit();
    }, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_X));
}

void MainWindow::initializeImporterExporters()
{
    ui->menu_Import_Bits_From->clear();
    ui->menu_Export_Bits_To->clear();
    for (QSharedPointer<ImporterExporterInterface> plugin : m_pluginManager->importerExporters()) {
        if (plugin->canImport()) {
            ui->menu_Import_Bits_From->setEnabled(true);
            ui->menu_Import_Bits_From->addAction(
                    plugin->name(),
                    [this, plugin]() {
                Parameters parameters = ParameterEditorDialog::promptForParameters(plugin->importParameterDelegate());
                if (!parameters.isNull()) {
                    this->requestImportRun(plugin->name(), parameters);
                }
            });
        }
        if (plugin->canExport()) {
            ui->menu_Export_Bits_To->setEnabled(true);
            ui->menu_Export_Bits_To->addAction(
                    plugin->name(),
                    [this, plugin]() {
                Parameters parameters = ParameterEditorDialog::promptForParameters(plugin->exportParameterDelegate());
                if (!parameters.isNull()) {
                    this->requestExportRun(plugin->name(), parameters);
                }
            });
        }
    }
}

void MainWindow::warningMessage(QString message, QString windowTitle)
{
    QMessageBox msg;
    msg.setWindowTitle(windowTitle);
    msg.setText(message);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

QSharedPointer<BitContainer> MainWindow::currContainer()
{
    return m_bitContainerManager->currentContainer();
}

void MainWindow::importBitfile(QString file)
{
    QSharedPointer<ImporterExporterInterface> fileDataImporter = m_pluginManager->getImporterExporter("File Data");
    if (fileDataImporter.isNull()) {
        warningMessage("Could not import bit file without 'File Data' plugin");
        return;
    }
    Parameters parameters;
    parameters.insert("filename", file);
    requestImportRun("File Data", parameters);
}

void MainWindow::importBytes(QByteArray rawBytes, QString name)
{
    auto bitContainer = BitContainer::create(rawBytes);
    bitContainer->setName(name);

    QModelIndex addedIndex = m_bitContainerManager->getTreeModel()->addContainer(bitContainer);
    m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
            addedIndex,
            QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_pb_operate_clicked()
{
    QSharedPointer<OperatorInterface> op = getCurrentOperator();
    if (op.isNull()) {
        warningMessage("Current Operator plugin cannot be determined");
        return;
    }
    AbstractParameterEditor *editor = m_operatorUiMap.value(op);
    if (editor == nullptr) {
        warningMessage("No editor initialized for plugin " + op->name());
        return;
    }
    Parameters parameters = editor->parameters();
    this->requestOperatorRun(op->name(), parameters);
}

void MainWindow::checkOperatorInput(QString pluginName)
{
    QSharedPointer<OperatorInterface> op = getCurrentOperator();
    if (op.isNull()) {
        ui->pb_operate->setEnabled(false);
        return;
    }

    if (!pluginName.isEmpty()) {
        if (pluginName != op->name()) {
            return;
        }
    }

    AbstractParameterEditor *editor = m_operatorUiMap.value(op);
    if (editor == nullptr) {
        ui->pb_operate->setEnabled(false);
        return;
    }

    // TODO: either set this up in AbstractParameterEditor, or maybe just always have the button enabled
    //ui->pb_operate->setEnabled(op->parameterDelegate()->validate(editor->parameters()));

    ui->pb_operate->setEnabled(true);
}

QSharedPointer<OperatorInterface> MainWindow::getCurrentOperator()
{
    return m_operatorMap.value(ui->operatorTabs->currentIndex(), QSharedPointer<OperatorInterface>());
}

void MainWindow::loadPlugins()
{
    QVariant badPluginPaths = SettingsManager::getPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY);
    if (badPluginPaths.isValid() && badPluginPaths.canConvert<QStringList>()) {
        QStringList badList = badPluginPaths.toStringList();
        badList.removeDuplicates();
        for (auto badPluginPath : badList) {
            if (QMessageBox::question(
                    this,
                    "Blacklist Plugin?",
                    QString(
                            "The plugin at '%1' was running when the application terminated.  Do you want to blacklist this plugin? You can always edit the blacklist in the Preferences.")
                    .arg(badPluginPath)) == QMessageBox::Yes) {
                QVariant oldBlacklist = SettingsManager::getPluginLoaderSetting(
                        SettingsManager::PLUGIN_BLACKLIST_KEY);
                QStringList blacklist;
                if (oldBlacklist.isValid() && oldBlacklist.canConvert<QStringList>()) {
                    blacklist = oldBlacklist.toStringList();
                }
                blacklist.append(badPluginPath);
                SettingsManager::setPluginLoaderSetting(SettingsManager::PLUGIN_BLACKLIST_KEY, blacklist);
            }
        }
    }
    SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList());

    QStringList warnings;
    QStringList pluginPaths;
    if (!m_extraPluginPath.isEmpty()) {
        pluginPaths.append(m_extraPluginPath.split(":"));
    }
    pluginPaths.append(
            SettingsManager::getPluginLoaderSetting(
                    SettingsManager::PLUGIN_PATH_KEY).toString().split(":"));

    QStringList pathBuffer;
    for (QString pluginPath : pluginPaths) {

        if (pluginPath.startsWith("~/")) {
            pluginPath.replace(0, 1, QDir::homePath());
        }
        else if (!pluginPath.startsWith("/")) {
            pluginPath = QApplication::applicationDirPath() + "/" + pluginPath;
        }
        pathBuffer.append(pluginPath);
    }
    pluginPaths = pathBuffer;

    for (QString pluginPath : pluginPaths) {
        warnings.append(m_pluginManager->loadPlugins(pluginPath));
    }

    if (!warnings.isEmpty()) {
        QMessageBox msg;
        msg.setWindowTitle("Plugin Load Warnings");
        msg.setText(warnings.join("\n"));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }

    warnings.clear();
    for (QString pluginPath : pluginPaths) {
        warnings.append(PythonPluginConfig::loadPythonPlugins(pluginPath, m_pluginManager, [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
            Q_UNUSED(size)
            return new SimpleParameterEditor(delegate, "Set Parameters");
        }));
    }

    if (!warnings.isEmpty()) {
        QMessageBox msg;
        msg.setWindowTitle("Python Plugin Load Warnings");
        msg.setText(warnings.join("\n"));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }


    QSet<QString> queued;

    ui->operatorTabs->setUpdatesEnabled(false);
    QList<QSharedPointer<OperatorInterface>> operators;
    queued.clear();
    for (QString pluginString : SettingsManager::getPluginLoaderSetting(
            SettingsManager::OPERATOR_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<OperatorInterface> plugin = m_pluginManager->getOperator(pluginString.trimmed());
        if (!plugin.isNull()) {
            operators.append(plugin);
            queued.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<OperatorInterface> plugin : m_pluginManager->operators()) {
        if (!queued.contains(plugin->name())) {
            operators.append(plugin);
        }
    }
    for (QSharedPointer<OperatorInterface> op : operators) {
        AbstractParameterEditor *opUi = op->parameterDelegate()->createEditor();
        if (opUi == nullptr) {
            continue;
        }
        opUi->setWhatsThis(QString("<b>%1:</b> %2").arg(op->name()).arg(op->description()));
        int idx = ui->operatorTabs->addTab(opUi, op->name());
        m_operatorMap.insert(idx, op);
        m_operatorUiMap.insert(op, opUi);

        opUi->giveDisplayHandle(m_displayHandle);
        connect(opUi, &AbstractParameterEditor::accepted, [this, opUi, op]() {
            this->requestOperatorRun(op->name(), opUi->parameters());
        });
    }
    ui->operatorTabs->setUpdatesEnabled(true);

    ui->analyzerTabs->setUpdatesEnabled(false);
    QList<QSharedPointer<AnalyzerInterface>> analyzers;
    queued.clear();
    for (QString pluginString : SettingsManager::getPluginLoaderSetting(
            SettingsManager::ANALYZER_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<AnalyzerInterface> plugin = m_pluginManager->getAnalyzer(pluginString.trimmed());
        if (!plugin.isNull()) {
            analyzers.append(plugin);
            queued.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<AnalyzerInterface> plugin : m_pluginManager->analyzers()) {
        if (!queued.contains(plugin->name())) {
            analyzers.append(plugin);
        }
    }

    for (QSharedPointer<AnalyzerInterface> analyzer : analyzers) {
        AbstractParameterEditor *analysisUi = analyzer->parameterDelegate()->createEditor();
        if (analysisUi == nullptr) {
            continue;
        }
        analysisUi->setWhatsThis(QString("<b>%1:</b> %2").arg(analyzer->name()).arg(analyzer->description()));
        int idx = ui->analyzerTabs->addTab(analysisUi, analyzer->name());
        m_analyzerMap.insert(idx, analyzer);
        m_analyzerUiMap.insert(analyzer, analysisUi);

        analysisUi->giveDisplayHandle(m_displayHandle);
        connect(analysisUi, &AbstractParameterEditor::accepted, [this, analysisUi, analyzer] {
            this->requestAnalyzerRun(analyzer->name(), analysisUi->parameters());
        });
    }
    ui->analyzerTabs->setUpdatesEnabled(true);
}

void MainWindow::activateBitContainer(QSharedPointer<BitContainer> selected, QSharedPointer<BitContainer> deselected)
{
    if (!deselected.isNull()) {
        disconnect(deselected.data(), SIGNAL(changed()), this, SLOT(currBitContainerChanged()));
    }
    if (!selected.isNull()) {
        connect(selected.data(), SIGNAL(changed()), this, SLOT(currBitContainerChanged()));
    }

    QSharedPointer<BitContainer> bitContainer = currContainer();
    if (bitContainer.isNull()) {
        ui->tb_removeBitContainer->setEnabled(false);
    }
    else {
        ui->tb_removeBitContainer->setEnabled(true);
    }
    setCurrentBitContainer();
}

void MainWindow::currBitContainerChanged()
{
    sendBitContainerPreview();
    checkOperatorInput();
}

void MainWindow::sendBitContainerPreview()
{
    QSharedPointer<BitContainerPreview> preview;
    if (!currContainer().isNull()) {
        preview = BitContainerPreview::wrap(currContainer());
    }
    for (auto editor : m_analyzerUiMap.values()) {
        QtConcurrent::run(MainWindow::processBitPreview, preview, editor);
    }
    for (auto editor : m_operatorUiMap.values()) {
        QtConcurrent::run(MainWindow::processBitPreview, preview, editor);
    }
}

void MainWindow::processBitPreview(QSharedPointer<BitContainerPreview> preview, AbstractParameterEditor* editor)
{
    // TODO: show preview progress
    QSharedPointer<PluginActionProgress> progress(new PluginActionProgress());
    editor->previewBits(preview, progress);
}

void MainWindow::setCurrentBitContainer()
{
    currBitContainerChanged();

    if (!currContainer().isNull()) {
        // Set the operator plugin settings used on this container
        if (!currContainer()->childUuids().isEmpty()) {
            QSet<QString> alreadySet;
            auto outputs = currContainer()->actionLineage()->outputOperators();
            for (int i = outputs.size() - 1; i >= 0; i--) {
                auto output = outputs.at(i);
                if (alreadySet.contains(output->pluginName())) {
                    continue;
                }
                auto op = m_pluginManager->getOperator(output->pluginName());
                if (!op.isNull()) {
                    AbstractParameterEditor* editor = m_operatorUiMap.value(op);
                    if (editor != nullptr) {
                        Parameters parameters = output->parameters();
                        editor->setParameters(parameters);
                        alreadySet.insert(output->pluginName());
                    }
                }
            }
        }
    }
}

const QString CONFIRM_BITS_DELETION = "Confirm Bits Deletion";

void MainWindow::deleteCurrentBitcontainer()
{
    if (!currContainer().isNull()) {
        QMessageBox::StandardButton reply;
        auto confirm = SettingsManager::getUiSetting(CONFIRM_BITS_DELETION);
        if (!confirm.isValid() || confirm.toBool()) {
            reply = QMessageBox::question(
                this,
                "Delete Bits Confirmation",
                QString("Are you sure you want to delete bit container '%1'?").arg(currContainer()->name()),
                QMessageBox::YesToAll | QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::YesToAll) {
                SettingsManager::setUiSetting(CONFIRM_BITS_DELETION, QVariant(false));
            }
            else if (reply != QMessageBox::Yes) {
                return;
            }
        }

        ui->tb_removeBitContainer->setEnabled(false);
        m_bitContainerManager->deleteCurrentContainer();
    }
}

void MainWindow::deleteAllBitContainers()
{
    QMessageBox::StandardButton reply;
    auto confirm = SettingsManager::getUiSetting(CONFIRM_BITS_DELETION);
    if (!confirm.isValid() || confirm.toBool()) {
        reply = QMessageBox::question(
            this,
            "Delete Bits Confirmation",
            QString("Are you sure you want to delete all bit containers?"),
            QMessageBox::YesToAll | QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::YesToAll) {
            SettingsManager::setUiSetting(CONFIRM_BITS_DELETION, QVariant(false));
        }
        else if (reply != QMessageBox::Yes) {
            return;
        }
    }

    ui->tb_removeBitContainer->setEnabled(false);
    m_bitContainerManager->deleteAllContainers();
}

void MainWindow::setStatus(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::requestAnalyzerRun(QString pluginName, const Parameters &parameters)
{
    if (!currContainer().isNull()) {
        m_pluginActionManager->runAnalyzer(PluginAction::analyzerAction(pluginName, parameters), currContainer());
    }
}

void MainWindow::requestOperatorRun(QString pluginName, const Parameters &parameters)
{
    if (!currContainer().isNull()) {
        QSharedPointer<OperatorInterface> plugin = m_pluginManager->getOperator(pluginName);
        if (!plugin.isNull()) {

            QList<QSharedPointer<BitContainer>> inputContainers;
            int minInputs = plugin->getMinInputContainers(parameters);
            int maxInputs = plugin->getMaxInputContainers(parameters);
            if (maxInputs == 1 && minInputs == 1) {
                inputContainers.append(currContainer());
            }
            else if (maxInputs > 1) {
                ContainerSelectionDialog *selectionDialog = new ContainerSelectionDialog(m_bitContainerManager, this);
                if (!selectionDialog->exec()) {
                    return;
                }
                inputContainers = selectionDialog->getSelected();
                if (inputContainers.size() < minInputs
                    || inputContainers.size() > maxInputs) {
                    warningMessage(
                            QString("You must select between %1 and %2 input containers (%3 selected)")
                            .arg(minInputs).arg(maxInputs).arg(inputContainers.size()),
                            "Invalid Input Count");
                    return;
                }
            }

            m_pluginActionManager->runOperator(PluginAction::operatorAction(pluginName, parameters), inputContainers);
        }
    }
}

void MainWindow::requestImportRun(QString pluginName, const Parameters &parameters)
{
    auto runner = m_pluginActionManager->runImporter(PluginAction::importerAction(pluginName, parameters));
    if (runner.isNull()) {
        warningMessage(QString("Failed to initialize importer '%1'").arg(pluginName));
        return;
    }
    m_pendingImports.insert(runner->id(), runner);
}

void MainWindow::requestExportRun(QString pluginName, const Parameters &parameters)
{
    if (currContainer().isNull()) {
        warningMessage(QString("No container selected for export"));
        return;
    }
    auto runner = m_pluginActionManager->runExporter(PluginAction::exporterAction(pluginName, parameters), currContainer());
    if (runner.isNull()) {
        warningMessage(QString("Failed to initialize exporter '%1'").arg(pluginName));
        return;
    }
    m_pendingExports.insert(runner->id(), runner);
}

void MainWindow::on_pb_analyze_clicked()
{
    auto plugin = m_analyzerMap.value(ui->analyzerTabs->currentIndex(), QSharedPointer<AnalyzerInterface>());
    if (plugin.isNull()) {
        warningMessage("Current Analyzer plugin cannot be determined");
        return;
    }
    Parameters parameters = Parameters::nullParameters();
    AbstractParameterEditor *editor = m_analyzerUiMap.value(plugin);
    if (editor != nullptr) {
        parameters = editor->parameters();
    }
    requestAnalyzerRun(plugin->name(), parameters);
}

void MainWindow::on_action_Apply_Batch_triggered()
{
    QString fileName = WidgetsSettings::getFile(
        this,
        tr("Apply Batch"),
        QDir::homePath(),
        tr("Hobbits Batch Files (*.hbat)"),
        QFileDialog::AcceptOpen,
        QFileDialog::ExistingFile,
        SettingsManager::LAST_BATCH_KEY
    );

    if (!fileName.isEmpty()) {
        applyBatchFile(fileName);
    }
}

void MainWindow::on_action_Save_Batch_triggered()
{
    if (currContainer().isNull() || currContainer()->actionLineage().isNull()) {
        warningMessage(
                "You must first select a bit container with children or a history of plugin operations in order to save a batch.",
                "Cannot Save Batch");
        return;
    }

    auto batchDialog = QSharedPointer<BatchCreationDialog>(new BatchCreationDialog(currContainer(), this));

    if (!batchDialog->exec()) {
        return;
    }

    auto batch = PluginActionBatch::fromLineage(currContainer()->actionLineage(), batchDialog->getSelectedBatchMode());

    if (batch.isNull() || batch->actionSteps().isEmpty()) {
        warningMessage(
                "A valid batch could not be created from the container's relevant actions",
                "Cannot Save Batch");
        return;
    }

    m_batchEditor->setBatch(batch);
    m_batchEditor->show();
}

void MainWindow::applyBatchFile(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        warningMessage(QString("Could not open hobbits batch file '%1'").arg(fileName));
        return;
    }

    auto batch = PluginActionBatch::deserialize(QJsonDocument::fromJson(file.readAll()).object());

    if (batch.isNull()) {
        warningMessage(QString("Format of hobbits batch file could not be read '%1'").arg(fileName));
        return;
    }

    int requiredInputs = batch->getRequiredInputs();

    QList<QSharedPointer<BitContainer>> inputs;
    if (requiredInputs == 1 && !currContainer().isNull()) {
        inputs.append(currContainer());
    }
    else if (requiredInputs > 0) {
        auto selectionDialog = QSharedPointer<ContainerSelectionDialog>(new ContainerSelectionDialog(m_bitContainerManager, this));
        selectionDialog->setMessage(QString("Select %1 inputs for the batch").arg(requiredInputs));
        if (!selectionDialog->exec()) {
            return;
        }
        inputs = selectionDialog->getSelected();
        if (inputs.size() != requiredInputs) {
            warningMessage(
                    QString("You must select %1 input containers (%2 selected)")
                    .arg(requiredInputs).arg(inputs.size()),
                    "Invalid Input Count");
            return;
        }
    }

    populateRecentBatchesMenu(fileName);
    m_pluginActionManager->runBatch(batch, inputs);
}

void MainWindow::pluginActionStarted(QUuid id)
{
    auto progress = new PluginProgress;
    progress->id = id;
    progress->progressBar = new QProgressBar;
    progress->cancelButton = new QPushButton("Cancel");

    ui->statusBar->addPermanentWidget(progress->progressBar);
    ui->statusBar->addPermanentWidget(progress->cancelButton);

    connect(progress->cancelButton, &QPushButton::pressed, [this, progress]() {
        progress->cancelButton->setDisabled(true);
        this->m_pluginActionManager->cancelById(progress->id);
    });

    m_pluginProgress.insert(id, progress);
}

void MainWindow::pluginActionFinished(QUuid id)
{
    if (m_pendingImports.contains(id)) {
        auto runner = m_pendingImports.take(id);
        auto result = runner->result();
        if (!result.isNull()
                && !result->hasNullParameters()
                && result->errorString().isEmpty()) {
            this->populateRecentImportsMenu({runner->pluginName(), result->parameters()});
        }
    }
    else if (m_pendingExports.contains(id)) {
        auto runner = m_pendingExports.take(id);
        auto result = runner->result();
        if (!result.isNull()
                && !result->hasNullParameters()
                && result->errorString().isEmpty()) {
            this->populateRecentExportsMenu({runner->pluginName(), result->parameters()});
        }
    }

    if (!m_pluginProgress.contains(id)) {
        return;
    }
    auto progress = m_pluginProgress.value(id);
    progress->progressBar->deleteLater();
    progress->cancelButton->deleteLater();
    m_pluginProgress.remove(id);
    delete progress;
}

void MainWindow::pluginActionProgress(QUuid id, int progress)
{
    if (!m_pluginProgress.contains(id)) {
        return;
    }
    m_pluginProgress.value(id)->progressBar->setValue(progress);
}

#include "hobbitspython.h"

void MainWindow::on_action_About_triggered()
{
    QString coreLibVersion = HobbitsCoreConfig::VERSION;
    QString guiVersion = HobbitsGuiConfig::VERSION;

    QString info =  QString("Hobbits GUI Version: %1\nHobbits Core Version: %2").arg(guiVersion).arg(coreLibVersion);
    info += QString("\nIntegrated Python: %1").arg(HobbitsPythonConfig::PYTHON_VERSION);

    QMessageBox msg;
    msg.setWindowTitle("About Hobbits");
    msg.setText(info);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.setIconPixmap(QIcon(":/hobbitsgui/images/icons/HobbitsRingSmall.png").pixmap(64, 64));
    msg.exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(m_pluginManager);
    dialog.exec();
}

void MainWindow::populateRecentBatchesMenu(QString addition, QString removal)
{
    QString key = "recently_used_batches";

    QStringList recentlyUsed;
    QVariant currentSetting = SettingsManager::getPrivateSetting(key);
    if (!currentSetting.isNull() && currentSetting.canConvert<QStringList>()) {
        recentlyUsed = currentSetting.toStringList();
    }

    if (!removal.isEmpty()) {
        recentlyUsed.removeAll(removal);
    }

    if (!addition.isEmpty()) {
        recentlyUsed.removeAll(addition);
        recentlyUsed.insert(0, addition);
    }

    recentlyUsed = recentlyUsed.mid(0, 10);

    SettingsManager::setPrivateSetting(key, recentlyUsed);

    ui->menuApply_Recent_Batch->clear();
    for (QString batchFile : recentlyUsed) {
        ui->menuApply_Recent_Batch->addAction(
                batchFile,
                [this, batchFile]() {
            this->applyBatchFile(batchFile);
        });
    }

    ui->menuApply_Recent_Batch->setEnabled(recentlyUsed.length() > 0);
}


void MainWindow::populatePresetBatchesMenu()
{
    QMenu* builtInBatchMenu = ui->menuFile->addMenu("Apply Built-in Batch");
    
    for (QFileInfo batch: QDir(":/batches").entryInfoList()) {
        QString batchPath = batch.absoluteFilePath();
        builtInBatchMenu->addAction(
            batch.baseName(),
            [this, batchPath]() {
                this->applyBatchFile(batchPath);
            });
    }
}


void MainWindow::populateRecentImportsMenu(QPair<QString, Parameters> addition, QPair<QString, Parameters> removal)
{
    populatePluginActionMenu("recently_imported", ui->menuImport_Recent,
                             [this](QString pluginName, const Parameters &parameters){
        QSharedPointer<ImporterExporterInterface> plugin = this->m_pluginManager->getImporterExporter(pluginName);
        if (plugin.isNull()) {
            return QString();
        }
        return plugin->importParameterDelegate()->actionDescription(parameters);
    },
    [this](QString pluginName, const Parameters &parameters){
        this->requestImportRun(pluginName, parameters);
    },
    addition, removal);
}

void MainWindow::populateRecentExportsMenu(QPair<QString, Parameters> addition, QPair<QString, Parameters> removal)
{
    populatePluginActionMenu("recently_exported", ui->menuExport_Recent,
                             [this](QString pluginName, const Parameters &parameters){
        QSharedPointer<ImporterExporterInterface> plugin = this->m_pluginManager->getImporterExporter(pluginName);
        if (plugin.isNull()) {
            return QString();
        }
        return plugin->exportParameterDelegate()->actionDescription(parameters);
    },
    [this](QString pluginName, const Parameters &parameters){
        this->requestExportRun(pluginName, parameters);
    },
    addition, removal);
}

void MainWindow::populatePluginActionMenu(QString key, QMenu* menu,
                              const std::function<QString(QString, Parameters)> getLabel,
                              const std::function<void(QString, Parameters)> triggerAction,
                              QPair<QString, Parameters> addition,
                              QPair<QString, Parameters> removal)
{
    QString separator = "/[]\"[]/";

    QString additionString;
    if (!addition.first.isEmpty() && !addition.second.isNull()) {
        QJsonDocument doc(addition.second.values());
        QString additionJson(doc.toJson(QJsonDocument::Compact));
        additionString = QString("%1%2%3").arg(addition.first).arg(separator).arg(additionJson);
    }
    QString removalString;
    if (!removal.first.isEmpty() && !removal.second.isNull()) {
        QJsonDocument doc(removal.second.values());
        QString removalJson(doc.toJson(QJsonDocument::Compact));
        removalString = QString("%1%2%3").arg(removal.first).arg(separator).arg(removalJson);
    }

    QStringList recentlyExported;
    QVariant currentSetting = SettingsManager::getPrivateSetting(key);
    if (!currentSetting.isNull() && currentSetting.canConvert<QStringList>()) {
        recentlyExported = currentSetting.toStringList();
    }

    if (!removalString.isEmpty()) {
        recentlyExported.removeAll(removalString);
    }

    if (!additionString.isEmpty()) {
        recentlyExported.removeAll(additionString);
        recentlyExported.insert(0, additionString);
    }

    recentlyExported = recentlyExported.mid(0, 10);

    SettingsManager::setPrivateSetting(key, recentlyExported);

    int invalidStateCount = 0;
    menu->clear();
    for (QString importString : recentlyExported) {
        QStringList importParts = importString.split(separator);
        if (importParts.size() != 2) {
            invalidStateCount++;
            continue;
        }
        QString pluginName = importParts.at(0);
        QByteArray parametersString = importParts.at(1).toUtf8();
        Parameters parameters(QJsonDocument::fromJson(parametersString).object());

        QString menuLabel = getLabel(pluginName, parameters);
        if (menuLabel.isEmpty()) {
            invalidStateCount++;
            continue;
        }
        menu->addAction(
                menuLabel,
                [pluginName, parameters, triggerAction]() {
            triggerAction(pluginName, parameters);
        });
    }

    menu->setEnabled(recentlyExported.length() - invalidStateCount > 0);
}

void MainWindow::on_action_BatchEditor_triggered()
{
    m_batchEditor->show();
}

void MainWindow::on_action_Whats_This_triggered()
{
    QWhatsThis::enterWhatsThisMode();
}
