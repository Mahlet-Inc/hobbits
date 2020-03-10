#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPluginLoader>
#include <QCloseEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QQueue>
#include <QSettings>

#include "hobbitscoreinfo.h"
#include "hobbitsguiinfo.h"
#include "pluginactionlineage.h"
#include "preferencesdialog.h"
#include "settingsmanager.h"
#include "templatefilehandler.h"
#include "containerselectiondialog.h"

MainWindow::MainWindow(QString extraPluginPath, QString configFilePath, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_extraPluginPath(extraPluginPath),
    m_bitContainerManager(QSharedPointer<BitContainerManager>(new BitContainerManager())),
    m_pluginManager(QSharedPointer<PluginManager>(new PluginManager())),
    m_pluginActionManager(new PluginActionManager(m_pluginManager)),
    m_pluginActionProgress(new QProgressBar()),
    m_pluginActionCancel(new QPushButton()),
    m_displayTabsSplitter(new QSplitter(Qt::Horizontal)),
    m_splitViewMenu(new QMenu("Split View"))
{
    ui->setupUi(this);

    if (!configFilePath.isEmpty()) {
        SettingsManager::getInstance().setConfigFilePath(configFilePath);
    }
    resize(SettingsManager::getInstance().getPrivateSetting(SettingsData::WINDOW_SIZE_KEY).toSize());
    move(SettingsManager::getInstance().getPrivateSetting(SettingsData::WINDOW_POSITION_KEY).toPoint());

    // Populate View Menu
    ui->menu_View->addAction(ui->dock_bitContainerSelect->toggleViewAction());
    ui->menu_View->addAction(ui->dock_operatorPlugins->toggleViewAction());
    ui->menu_View->addSeparator();
    ui->menu_View->addAction(ui->dock_findBits->toggleViewAction());
    ui->menu_View->addSeparator();
    ui->menu_View->addMenu(m_splitViewMenu);

    ui->dock_bitContainerSelect->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B));
    ui->dock_operatorPlugins->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    ui->dock_findBits->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));

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

    // Configure Plugin Action Management
    connect(
            ui->operatorTabs,
            &QTabWidget::currentChanged,
            this,
            &MainWindow::checkOperatorInput);

    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::actionWatcherStarted,
            this,
            &MainWindow::pluginActionStarted);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::actionWatcherProgress,
            this,
            &MainWindow::pluginActionProgress);
    connect(
            m_pluginActionManager.data(),
            &PluginActionManager::actionWatcherFinished,
            this,
            &MainWindow::pluginActionFinished);

    connect(
            m_pluginActionManager.data(),
            SIGNAL(reportError(QString)),
            this,
            SLOT(warningMessage(QString)));

    m_pluginActionProgress->setVisible(false);
    ui->statusBar->addPermanentWidget(m_pluginActionProgress);

    m_pluginActionCancel->setVisible(false);
    m_pluginActionCancel->setText("Cancel");
    ui->statusBar->addPermanentWidget(m_pluginActionCancel);
    connect(
            m_pluginActionCancel,
            &QPushButton::pressed,
            m_pluginActionManager.data(),
            &PluginActionManager::cancelAction);

    m_analyzerPluginCallback = QSharedPointer<PluginCallback>(new PluginCallback());
    connect(
            m_analyzerPluginCallback.data(),
            &PluginCallback::runRequested,
            this,
            &MainWindow::requestAnalyzerRun);

    m_operatorPluginCallback = QSharedPointer<PluginCallback>(new PluginCallback());
    connect(
            m_operatorPluginCallback.data(),
            &PluginCallback::runRequested,
            this,
            &MainWindow::requestOperatorRun);

    //
    populateRecentTemplatesMenu();

    loadPlugins();

    initializeImporterExporters();

    m_displayHandle = QSharedPointer<DisplayHandle>(
            new DisplayHandle(
                    m_bitContainerManager,
                    ui->displayVScroll,
                    ui->displayHScroll));
    connect(
            m_displayHandle.data(),
            &DisplayHandle::newBitHover,
            this,
            &MainWindow::setHoverBit);
    initializeDisplays();

    checkOperatorInput();

    activateBitContainer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QString SPLIT_DISPLAY_SIZE_KEY = "split_display_size_list";
void MainWindow::closeEvent(QCloseEvent *event)
{
    SettingsManager::getInstance().setPrivateSetting(SettingsData::WINDOW_SIZE_KEY, size());
    SettingsManager::getInstance().setPrivateSetting(SettingsData::WINDOW_POSITION_KEY, pos());

    QStringList sizesAsStrings;
    for (int splitSize : m_displayTabsSplitter->sizes()) {
        sizesAsStrings.append(QString("%1").arg(splitSize));
    }
    SettingsManager::getInstance().setPrivateSetting(SPLIT_DISPLAY_SIZE_KEY, sizesAsStrings);

    event->accept();
}

void MainWindow::initializeDisplays()
{
    ui->displayTabsLayout->addWidget(m_displayTabsSplitter);
    m_displayTabsSplitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // initialize 1 no matter what
    addDisplayGroup();

    // Add others and sizes if they were saved in the config
    QVariant savedSizes = SettingsManager::getInstance().getPrivateSetting(SPLIT_DISPLAY_SIZE_KEY);
    if (!savedSizes.isNull() && savedSizes.canConvert<QStringList>()) {
        QList<int> splitSizes;
        bool ok = true;
        for (QString sizeString : savedSizes.toStringList()) {
            int splitSize = sizeString.toInt(&ok);
            if (!ok) {
                break;
            }
            splitSizes.append(splitSize);
        }
        if (ok && splitSizes.size() > 1) {
            for (int i = 1; i < splitSizes.size(); i++) {
                addDisplayGroup();
            }
            m_displayTabsSplitter->setSizes(splitSizes);
        }
    }
}

void MainWindow::addDisplayGroup()
{
    QTabWidget *tabs = new QTabWidget(this);
    tabs->setElideMode(Qt::ElideLeft);
    tabs->setDocumentMode(true);

    m_displayTabsSplitter->addWidget(tabs);

    // Instantiate displays for this display set
    QList<QSharedPointer<DisplayInterface>> displays;
    QSet<QString> queued;
    for (QString pluginString : SettingsManager::getInstance().getPluginLoaderSetting(
            SettingsData::DISPLAY_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<DisplayInterface> plugin = m_pluginManager->getDisplay(pluginString.trimmed());
        if (!plugin.isNull()) {
            displays.append(QSharedPointer<DisplayInterface>(plugin->createDefaultDisplay()));
            queued.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<DisplayInterface> plugin : m_pluginManager->getAllDisplays()) {
        if (!queued.contains(plugin->getName())) {
            displays.append(QSharedPointer<DisplayInterface>(plugin->createDefaultDisplay()));
        }
    }

    // Add the widgets to the tabs
    tabs->setUpdatesEnabled(false);
    QPair<QMap<int, QSharedPointer<DisplayInterface>>, QTabWidget*> displayMap;
    displayMap.second = tabs;
    for (QSharedPointer<DisplayInterface> displayPlugin : displays) {
        QWidget *display = displayPlugin->getDisplayWidget(m_displayHandle);
        int idx = tabs->addTab(display, displayPlugin->getName());
        displayMap.first.insert(idx, displayPlugin);
    }
    tabs->setUpdatesEnabled(true);

    // Add this display group to the list of displays
    m_displayMaps.append(displayMap);

    // Set up the display controls when necessary
    if (displayMap.first.size() > 0) {
        tabs->setCurrentIndex(0);
        checkCurrentDisplays();
    }
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(checkCurrentDisplays()));

    setupSplitViewMenu();
}

void MainWindow::removeDisplayGroup(int idx)
{
    if (idx <= 0 || idx >= m_displayMaps.length()) {
        warningMessage(QString("Cannot delete display group %1").arg(idx + 1));
        return;
    }

    m_displayMaps.at(idx).second->deleteLater();
    m_displayMaps.removeAt(idx);
    checkCurrentDisplays();

    setupSplitViewMenu();
}

void MainWindow::setupSplitViewMenu()
{
    m_splitViewMenu->clear();

    if (m_displayMaps.size() < 5) {
        m_splitViewMenu->addAction(
                "Add split view",
                [this]() {
            this->addDisplayGroup();
        })->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));
    }

    for (int i = 1; i < m_displayMaps.size(); i++) {
        QAction *remove = m_splitViewMenu->addAction(
                QString("Remove split view %1").arg(i + 1),
                [this, i]() {
            this->removeDisplayGroup(i);
        });
        if (i == m_displayMaps.size() - 1) {
            remove->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_X));
        }
    }
}

void MainWindow::initializeImporterExporters()
{
    ui->menu_Import_Bits_From->clear();
    ui->menu_Export_Bits_To->clear();
    for (QSharedPointer<ImportExportInterface> plugin : m_pluginManager->getAllImporterExporters()) {
        if (plugin->canImport()) {
            ui->menu_Import_Bits_From->setEnabled(true);
            ui->menu_Import_Bits_From->addAction(
                    plugin->getName(),
                    [this, plugin]() {
                QSharedPointer<BitContainer> container = plugin->importBits(QMap<QString, QString>(), this);
                if (!container.isNull()) {
                    QModelIndex addedIndex = this->m_bitContainerManager->getTreeModel()->addContainer(container);
                    this->m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
                            addedIndex,
                            QItemSelectionModel::ClearAndSelect);
                }
            });
        }
        if (plugin->canExport()) {
            ui->menu_Export_Bits_To->setEnabled(true);
            ui->menu_Export_Bits_To->addAction(
                    plugin->getName(),
                    [this, plugin]() {
                plugin->exportBits(this->currContainer(), QMap<QString, QString>(), this);
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

void MainWindow::checkCurrentDisplays()
{

    for (auto controls : m_currControlWidgets) {
        controls->setVisible(false);
        ui->displayControlsLayout->removeWidget(controls);
    }
    m_currControlWidgets.clear();

    QSet<DisplayInterface*> focusDisplays;
    for (auto displayMap : m_displayMaps) {
        QSharedPointer<DisplayInterface> currDisplay = displayMap.first.value(
                displayMap.second->currentIndex());
        if (!currDisplay.isNull()) {
            focusDisplays.insert(currDisplay.data());

            m_currControlWidgets.append(currDisplay->getControlsWidget(m_displayHandle));
        }
    }
    m_displayHandle->setFocusDisplays(focusDisplays);

    for (auto controls : m_currControlWidgets) {
        ui->displayControlsLayout->addWidget(controls);
        controls->setVisible(true);
    }
}

QSharedPointer<BitContainer> MainWindow::currContainer()
{
    return m_bitContainerManager->getCurrentContainer();
}

void MainWindow::importBitfile(QString file)
{
    QMap<QString, QString> args;
    args.insert("filename", file);

    QSharedPointer<ImportExportInterface> fileDataImporter = m_pluginManager->getImporterExporter("File Data");
    if (fileDataImporter.isNull()) {
        warningMessage("Could not import bit file without 'File Data' plugin");
        return;
    }
    QSharedPointer<BitContainer> container = fileDataImporter->importBits(args, this);
    if (!container.isNull()) {
        QModelIndex addedIndex = this->m_bitContainerManager->getTreeModel()->addContainer(container);
        this->m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
                addedIndex,
                QItemSelectionModel::ClearAndSelect);
    }
}

void MainWindow::importBytes(QByteArray rawBytes, QString name)
{
    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBytes(rawBytes);

    bitContainer->setName(name);

    QModelIndex addedIndex = m_bitContainerManager->getTreeModel()->addContainer(bitContainer);
    m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
            addedIndex,
            QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_pb_operate_clicked()
{
    QSharedPointer<OperatorInterface> op = getCurrentOperator();
    QJsonObject pluginState = op->getStateFromUi();
    this->requestOperatorRun(op->getName(), pluginState);
}

void MainWindow::checkOperatorInput()
{
    QSharedPointer<OperatorInterface> op = getCurrentOperator();
    if (op.isNull()) {
        ui->pb_operate->setEnabled(false);
        return;
    }

    QJsonObject pluginState = op->getStateFromUi();
    if (pluginState.isEmpty()) {
        ui->pb_operate->setEnabled(false);
    }
    else {
        ui->pb_operate->setEnabled(true);
    }
}

QSharedPointer<OperatorInterface> MainWindow::getCurrentOperator()
{
    return m_operatorMap.value(ui->operatorTabs->currentIndex(), QSharedPointer<OperatorInterface>());
}

void MainWindow::loadPlugins()
{
    QVariant badPluginPath = SettingsManager::getInstance().getPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY);
    if (badPluginPath.isValid()) {
        if (QMessageBox::question(
                this,
                "Blacklist Plugin?",
                QString(
                        "The plugin at '%1' was running when the application terminated.  Do you want to blacklist this plugin? You can always edit the blacklist in the Preferences.")
                .arg(badPluginPath.toString())) == QMessageBox::Yes) {
            QVariant oldBlacklist = SettingsManager::getInstance().getPluginLoaderSetting(
                    SettingsData::PLUGIN_BLACKLIST_KEY);
            QStringList blacklist;
            if (oldBlacklist.isValid() && oldBlacklist.canConvert<QStringList>()) {
                blacklist = oldBlacklist.toStringList();
            }
            blacklist.append(badPluginPath.toString());
            SettingsManager::getInstance().setPluginLoaderSetting(SettingsData::PLUGIN_BLACKLIST_KEY, blacklist);
        }
    }

    QStringList warnings;
    QStringList pluginPaths;
    if (!m_extraPluginPath.isEmpty()) {
        pluginPaths.append(m_extraPluginPath.split(":"));
    }
    pluginPaths.append(
            SettingsManager::getInstance().getPluginLoaderSetting(
                    SettingsData::PLUGIN_PATH_KEY).toString().split(":"));
    for (QString pluginPath : pluginPaths) {

        if (pluginPath.startsWith("~/")) {
            pluginPath.replace(0, 1, QDir::homePath());
        }
        else if (!pluginPath.startsWith("/")) {
            pluginPath = QApplication::applicationDirPath() + "/" + pluginPath;
        }
        warnings.append(m_pluginManager->loadPlugins(pluginPath));
    }

    if (!warnings.isEmpty()) {
        QMessageBox msg;
        msg.setWindowTitle("Plugin Load Warnings");
        msg.setText(warnings.join("\n"));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }


    QSet<QString> queued;

    ui->operatorTabs->setUpdatesEnabled(false);
    QList<QSharedPointer<OperatorInterface>> operators;
    queued.clear();
    for (QString pluginString : SettingsManager::getInstance().getPluginLoaderSetting(
            SettingsData::OPERATOR_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<OperatorInterface> plugin = m_pluginManager->getOperator(pluginString.trimmed());
        if (!plugin.isNull()) {
            operators.append(plugin);
            queued.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<OperatorInterface> plugin : m_pluginManager->getAllOperators()) {
        if (!queued.contains(plugin->getName())) {
            operators.append(plugin);
        }
    }
    for (QSharedPointer<OperatorInterface> op : operators) {
        QWidget *opUi = new QWidget();
        op->applyToWidget(opUi);
        opUi->setAutoFillBackground(true);
        int idx = ui->operatorTabs->addTab(opUi, op->getName());
        m_operatorMap.insert(idx, op);
        op->provideCallback(m_operatorPluginCallback);
    }
    ui->operatorTabs->setUpdatesEnabled(true);

    ui->analyzerTabs->setUpdatesEnabled(false);
    QList<QSharedPointer<AnalyzerInterface>> analyzers;
    queued.clear();
    for (QString pluginString : SettingsManager::getInstance().getPluginLoaderSetting(
            SettingsData::ANALYZER_DISPLAY_ORDER_KEY).toStringList()) {
        QSharedPointer<AnalyzerInterface> plugin = m_pluginManager->getAnalyzer(pluginString.trimmed());
        if (!plugin.isNull()) {
            analyzers.append(plugin);
            queued.insert(pluginString.trimmed());
        }
    }
    for (QSharedPointer<AnalyzerInterface> plugin : m_pluginManager->getAllAnalyzers()) {
        if (!queued.contains(plugin->getName())) {
            analyzers.append(plugin);
        }
    }

    for (QSharedPointer<AnalyzerInterface> analyzer : analyzers) {
        QWidget *analysisUi = new QWidget();
        analyzer->applyToWidget(analysisUi);
        analysisUi->setAutoFillBackground(true);
        int idx = ui->analyzerTabs->addTab(analysisUi, analyzer->getName());
        m_analyzerMap.insert(idx, analyzer);
        analyzer->provideCallback(m_analyzerPluginCallback);
    }
    ui->analyzerTabs->setUpdatesEnabled(true);
}

void MainWindow::activateBitContainer()
{
    QSharedPointer<BitContainer> bitContainer = currContainer();
    if (bitContainer.isNull()) {
        ui->tb_removeBitContainer->setEnabled(false);
    }
    else {
        ui->tb_removeBitContainer->setEnabled(true);
    }
    setCurrentBitContainer();
}

void MainWindow::setCurrentBitContainer()
{
    QSharedPointer<BitContainer> container = currContainer();

    disconnect(this, SIGNAL(containerFocusRequested(int,int)));

    if (!container.isNull()) {
        connect(
                container.data(),
                SIGNAL(focusRequested(int,int)),
                this,
                SLOT(
                        containerFocusRequested(
                                int,
                                int)));
    }

    for (QSharedPointer<AnalyzerInterface> analyzer : m_pluginManager->getAllAnalyzers()) {
        if (currContainer().isNull()) {
            analyzer->previewBits(QSharedPointer<BitContainerPreview>());
        }
        else {
            analyzer->previewBits(
                    QSharedPointer<BitContainerPreview>(
                            new BitContainerPreview(
                                    currContainer())));
        }
    }

    if (!currContainer().isNull()) {
        // Set the last analyzer plugin settings used on this container
        if (!currContainer()->getActionLineage().isNull()) {
            QSet<QString> usedPlugin;
            auto lineage = currContainer()->getActionLineage()->getLineage();
            for (int i = lineage.size() - 1; i >= 0; i--) {
                if (lineage.at(i)->getPluginAction()->getPluginType() == PluginAction::Analyzer) {
                    QString pluginName = lineage.at(i)->getPluginAction()->getPluginName();
                    if (usedPlugin.contains(pluginName)) {
                        break;
                    }
                    QJsonObject pluginState = lineage.at(i)->getPluginAction()->getPluginState();

                    auto analyzer = m_pluginManager->getAnalyzer(pluginName);
                    if (!analyzer.isNull()) {
                        if (analyzer->setPluginStateInUi(pluginState)) {
                            usedPlugin.insert(pluginName);
                        }
                    }
                }
                else {
                    break;
                }
            }
        }

        // Set the last operator plugin settings used on this container
        if (!currContainer()->getChildUuids().isEmpty()) {
            QSet<QString> usedPlugin;
            auto childId = currContainer()->getChildUuids().constEnd();
            while (childId != currContainer()->getChildUuids().constBegin()) {
                --childId;
                auto child = m_bitContainerManager->getTreeModel()->getContainerById(*childId);
                if (!child.isNull() && !child->getActionLineage().isNull()) {
                    auto lineage = child->getActionLineage()->getLineage();
                    for (int i = lineage.size() - 1; i >= 0; i--) {
                        if (lineage.at(i)->getPluginAction()->getPluginType() == PluginAction::Operator) {
                            QString pluginName = lineage.at(i)->getPluginAction()->getPluginName();
                            if (usedPlugin.contains(pluginName)) {
                                break;
                            }
                            QJsonObject pluginState = lineage.at(i)->getPluginAction()->getPluginState();

                            auto op = m_pluginManager->getOperator(pluginName);
                            if (!op.isNull()) {
                                if (op->setPluginStateInUi(pluginState)) {
                                    usedPlugin.insert(pluginName);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::containerFocusRequested(int bitOffset, int frameOffset)
{
    m_displayHandle->setOffsets(bitOffset, frameOffset);
}

void MainWindow::deleteCurrentBitcontainer()
{
    if (!currContainer().isNull()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
                this,
                "Delete Bits Confirmation",
                QString("Are you sure you want to delete bit container '%1'?").arg(currContainer()->getName()),
                QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        ui->tb_removeBitContainer->setEnabled(false);
        m_bitContainerManager->deleteCurrentContainer();
        activateBitContainer();
    }
}

void MainWindow::requestAnalyzerRun(QString pluginName, QJsonObject pluginState)
{
    if (!currContainer().isNull()) {
        QSharedPointer<AnalyzerInterface> plugin = m_pluginManager->getAnalyzer(pluginName);
        if (!plugin.isNull()) {
            m_pluginActionManager->analyzerActor()->analyzerFullAct(plugin, currContainer(), pluginState);
        }
    }
}

void MainWindow::requestOperatorRun(QString pluginName, QJsonObject pluginState)
{
    if (!currContainer().isNull()) {
        QSharedPointer<OperatorInterface> plugin = m_pluginManager->getOperator(pluginName);
        if (!plugin.isNull()) {
            QList<QSharedPointer<BitContainer>> inputContainers;
            QJsonObject pluginState = plugin->getStateFromUi();
            int minInputs = plugin->getMinInputContainers(pluginState);
            int maxInputs = plugin->getMaxInputContainers(pluginState);
            if (maxInputs == 1 && minInputs == 1) {
                inputContainers.append(currContainer());
            }
            else if (maxInputs > 1) {
                ContainerSelectionDialog* selectionDialog = new ContainerSelectionDialog(m_bitContainerManager, this);
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
            if (pluginState.isEmpty()) {
                pluginState = plugin->getStateFromUi();
                if (pluginState.isEmpty()) {
                    warningMessage(
                            "The plugin is unable to act due to a bad input value",
                            "Bad Plugin Input");
                    return;
                }
            }

            m_pluginActionManager->operatorActor()->operatorFullAct(
                    plugin,
                    inputContainers,
                    m_bitContainerManager,
                    ui->le_outputName->text());
        }
    }
}

void MainWindow::on_pb_analyze_clicked()
{
    if (!currContainer().isNull()) {
        m_pluginActionManager->analyzerActor()->analyzerFullAct(
                m_analyzerMap.value(ui->analyzerTabs->currentIndex(), QSharedPointer<AnalyzerInterface>()),
                currContainer());
    }
}

void MainWindow::setHoverBit(bool hovering, int bitOffset, int frameOffset)
{
    if (!hovering || frameOffset < 0 || bitOffset < 0 || currContainer().isNull()) {
        this->statusBar()->showMessage("");
    }
    else {
        int totalBitOffset = currContainer()->getFrames().at(frameOffset).start() + bitOffset;
        int totalByteOffset = totalBitOffset / 8;
        this->statusBar()->showMessage(
                QString("Bit Offset: %L1  Byte Offset: %L2  Frame Offset: %L3  Frame Bit Offset: %L4").arg(
                        totalBitOffset).arg(totalByteOffset).arg(frameOffset).arg(bitOffset));
    }
}

void MainWindow::on_action_Save_Current_Container_triggered()
{
    if (currContainer().isNull()) {
        warningMessage(
                "You must first select a bit container in order to save it.",
                "Cannot Save Container");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
            this,
            tr("Save Bit Container"),
            SettingsManager::getInstance().getPrivateSetting(
                    SettingsData::LAST_CONTAINER_PATH_KEY).toString(),
            tr("Hobbits Bit Containers (*.hobbits_bits)"));
    if (!fileName.endsWith(".hobbits_bits")) {
        fileName += ".hobbits_bits";
    }

    QFile file(fileName);
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_CONTAINER_PATH_KEY,
            QFileInfo(file).dir().path());

    if (!file.open(QIODevice::WriteOnly)) {
        warningMessage(
                QString("Could not open file '%1' for writing").arg(fileName),
                "Cannot Save Container");
        return;
    }

    QDataStream out(&file);
    out << *currContainer().data();
    file.close();

    if (out.status() != QDataStream::Status::Ok) {
        warningMessage(
                QString("Encountered errors while writing to file '%1'").arg(fileName),
                "Error Saving Container");
    }

}

QStringList MainWindow::openHobbitsBits(QString fileName)
{
    QFile file(fileName);
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_CONTAINER_PATH_KEY,
            QFileInfo(file).dir().path());

    if (!file.open(QIODevice::ReadOnly)) {
        return QStringList(QString("Could not open hobbits bits file '%1'").arg(fileName));
    }


    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    QDataStream in(&file);
    in >> *bitContainer.data();
    file.close();

    if (in.status() != QDataStream::Status::Ok) {
        return QStringList(
                QString("Failure opening hobbits container file '%1'\nMaybe you meant to 'Import Bits'?").arg(
                        fileName));
    }

    bitContainer->setName(QFileInfo(file).completeBaseName());

    QModelIndex addedIndex = m_bitContainerManager->getTreeModel()->addContainer(bitContainer);
    m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
            addedIndex,
            QItemSelectionModel::ClearAndSelect);

    return QStringList();
}

void MainWindow::on_actionOpen_Container_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open Bit Container"),
            SettingsManager::getInstance().getPrivateSetting(
                    SettingsData::LAST_CONTAINER_PATH_KEY).toString(),
            tr("Hobbits Bit Containers (*.hobbits_bits)"));

    if (fileName.isEmpty()) {
        return;
    }

    QStringList errors = openHobbitsBits(fileName);

    if (!errors.isEmpty()) {
        warningMessage(errors.join("\n\n"), "Error Opening Container");
    }
}

void MainWindow::on_action_Export_Template_triggered()
{
    if (currContainer().isNull() || (currContainer()->getActionLineage().isNull()
                                     && currContainer()->getChildUuids().isEmpty())) {
        warningMessage(
                "You must first select a bit container with children or a history of plugin operations in order to save a template.",
                "Cannot Save Template");
        return;
    }

    bool useChildren = false;
    if (currContainer()->getActionLineage().isNull()) {
        useChildren = true;
    }
    else if (currContainer()->getChildUuids().isEmpty()) {
        useChildren = false;
    }
    else {
        QMessageBox pickMode;
        pickMode.setWindowTitle("Select template type");
        pickMode.setText(
                "Do you want to create a template for the actions that created the selected container, or for the actions that created its children?");
        pickMode.addButton("Selected Container", QMessageBox::ButtonRole::AcceptRole);
        QPushButton *children = pickMode.addButton("Its Children", QMessageBox::ButtonRole::RejectRole);
        pickMode.exec();
        useChildren = pickMode.clickedButton() == children;
    }

    QString fileName = QFileDialog::getSaveFileName(
            this,
            tr("Save Template"),
            SettingsManager::getInstance().getPrivateSetting(
                    SettingsData::LAST_TEMPLATE_PATH_KEY).toString(),
            tr("Hobbits Templates (*.hobbits_template)"));
    if (!fileName.endsWith(".hobbits_template")) {
        fileName += ".hobbits_template";
    }

    QList<QSharedPointer<const PluginActionLineage>> lineages;
    if (useChildren) {
        QQueue<QUuid> containers;
        containers.append(currContainer()->getId());
        while (!containers.isEmpty()) {
            QUuid id = containers.takeFirst();
            QSharedPointer<BitContainer> container = m_bitContainerManager->getTreeModel()->getContainerById(
                    id);
            if (container.isNull()) {
                continue;
            }
            if (container->getChildUuids().isEmpty()) {
                lineages.append(container->getActionLineage());
            }
            else {
                containers.append(container->getChildUuids());
            }
        }
    }
    else {
        lineages.append(currContainer()->getActionLineage());
    }

    QStringList warnings;
    if (TemplateFileHandler::writeTemplate(fileName, lineages, &warnings)) {
        populateRecentTemplatesMenu(fileName);
        if (!warnings.isEmpty()) {
            warningMessage(warnings.join("\n\n"), "Failed to Export Template");
        }
    }
    else if (!warnings.isEmpty()) {
        warningMessage(warnings.join("\n\n"));
    }
}

void MainWindow::applyTemplateToCurrentContainer(QString fileName)
{
    QStringList warnings;
    auto lineageTree = TemplateFileHandler::loadTemplate(fileName, &warnings);
    if (lineageTree.isNull()) {
        warningMessage(warnings.join("\n\n"), "Failed to Apply Template");
        return;
    }
    else if (!warnings.isEmpty()) {
        warningMessage(warnings.join("\n\n"));
    }

    QFileInfo fileInfo(fileName);
    QString templateName = fileInfo.fileName().section(".", 0, 0);
    QList<QUuid> additionalInputs;
    if (lineageTree->additionalInputCount() > 0) {
        ContainerSelectionDialog* selectionDialog = new ContainerSelectionDialog(m_bitContainerManager, this);
        selectionDialog->setMessage(QString("Select %1 additional inputs for the provided template").arg(lineageTree->additionalInputCount()));
        if (!selectionDialog->exec()) {
            return;
        }
        auto additionals = selectionDialog->getSelected();
        if (additionals.size() != lineageTree->additionalInputCount()) {
            warningMessage(
                        QString("You must select between %1 input containers (%2 selected)")
                        .arg(lineageTree->additionalInputCount()).arg(additionals.size()),
                        "Invalid Input Count");
            return;
        }
        for (auto additional : additionals) {
            additionalInputs.append(additional->getId());
        }
    }
    TemplateFileHandler::applyLineageTree(
                currContainer()->getId(),
                additionalInputs,
                lineageTree,
                templateName,
                m_bitContainerManager,
                m_pluginActionManager);
}

void MainWindow::on_actionApply_Template_triggered()
{
    if (currContainer().isNull()) {
        warningMessage(
                "You must first select a bit container to apply the template to.",
                "Cannot Apply Template");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Apply Template"),
            SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_TEMPLATE_PATH_KEY).toString(),
            tr("Hobbits Templates (*.hobbits_template)"));
    if (fileName.isEmpty()) {
        return;
    }

    applyTemplateToCurrentContainer(fileName);
}

void MainWindow::pluginActionStarted()
{
    ui->pb_analyze->setEnabled(false);
    ui->pb_operate->setEnabled(false);
    m_pluginActionProgress->setValue(0);
    m_pluginActionProgress->setVisible(true);
    m_pluginActionCancel->setVisible(true);
}

void MainWindow::pluginActionFinished()
{
    ui->pb_analyze->setEnabled(true);
    ui->pb_operate->setEnabled(true);
    m_pluginActionProgress->setVisible(false);
    m_pluginActionCancel->setVisible(false);

    for (QSharedPointer<AnalyzerInterface> analyzer : m_pluginManager->getAllAnalyzers()) {
        analyzer->previewBits(
                QSharedPointer<BitContainerPreview>(
                        new BitContainerPreview(
                                currContainer())));
    }
}

void MainWindow::pluginActionProgress(int progress)
{
    m_pluginActionProgress->setValue(progress);
}

void MainWindow::on_action_About_triggered()
{
    QString coreLibVersion = HobbitsCoreInfo::getLibVersion();
    QString guiVersion = HobbitsGuiInfo::getGuiVersion();

    QMessageBox msg;
    msg.setWindowTitle("About Hobbits");
    msg.setText(
            QString("Hobbits GUI Version: %1\nHobbits Core Version: %2").arg(guiVersion).arg(
                    coreLibVersion));
    msg.setDefaultButton(QMessageBox::Ok);
    msg.setIconPixmap(QIcon(":/hobbitsgui/images/icons/HobbitsRingSmall.png").pixmap(64, 64));
    msg.exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(m_pluginManager);
    dialog.exec();
}

void MainWindow::populateRecentTemplatesMenu(QString addition, QString removal)
{
    QString key = "recently_used_templates";

    QStringList recentlyUsed;
    QVariant currentSetting = SettingsManager::getInstance().getPrivateSetting(key);
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

    SettingsManager::getInstance().setPrivateSetting(key, recentlyUsed);

    ui->menuApply_Recent_Template->clear();
    for (QString templateFile : recentlyUsed) {
        ui->menuApply_Recent_Template->addAction(
                templateFile,
                [this, templateFile]() {
            this->applyTemplateToCurrentContainer(templateFile);
        });
    }

    ui->menuApply_Recent_Template->setEnabled(recentlyUsed.length() > 0);
}

void MainWindow::on_tb_scrollReset_clicked()
{
    m_displayHandle->setOffsets(0, 0);
}
