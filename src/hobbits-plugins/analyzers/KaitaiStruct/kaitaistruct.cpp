#include "kaitaistruct.h"
#include "ui_kaitaistruct.h"
#include "settingsmanager.h"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTemporaryDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QJsonArray>
#include <QDirIterator>
#include "hobbitspython.h"

const QString KAITAI_PATH_KEY = "kaitai_struct_compiler_path";
const QString KSY_PATH_KEY = "ksy_directory_path";
const QString KAITAI_STRUCT_CATEGORY = "kaitai_struct";
const QString KAITAI_RESULT_LABEL = "kaitai_struct_result_label";

KaitaiStruct::KaitaiStruct() :
    ui(new Ui::KaitaiStruct()),
    m_loadKsyMenu(nullptr),
    m_loadKsyPyMenu(nullptr),
    m_highlightNav(nullptr)
{
}

KaitaiStruct::~KaitaiStruct()
{
    delete m_loadKsyMenu;
    delete m_loadKsyPyMenu;
    delete ui;
}

AnalyzerInterface* KaitaiStruct::createDefaultAnalyzer()
{
    return new KaitaiStruct();
}

//Return name of operator
QString KaitaiStruct::name()
{
    return "Kaitai Struct";
}

void KaitaiStruct::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
    if (m_highlightNav) {
        m_highlightNav->setPluginCallback(m_pluginCallback);
    }
}

void KaitaiStruct::applyToWidget(QWidget *widget)
{
    m_loadKsyMenu = new QMenu();
    m_loadKsyMenu->addAction("Load File...", [this]() {
        QString fileName = QFileDialog::getOpenFileName(
                    nullptr,
                    tr("Select ksy File"),
                    SettingsManager::getInstance().getPrivateSetting(KSY_PATH_KEY).toString(),
                    tr("Kaitai Struct File (*.ksy);;All Files (*)"));
        if (fileName.isEmpty()) {
            return;
        }
        QFile ksyFile(fileName);
        if (!ksyFile.open(QIODevice::ReadOnly)) {
            return;
        }
        SettingsManager::getInstance().setPrivateSetting(KSY_PATH_KEY, QFileInfo(ksyFile).path());
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

    ui->setupUi(widget);

    ui->pb_loadKsy->setMenu(m_loadKsyMenu);
    ui->pb_selectPrecompiled->setMenu(m_loadKsyPyMenu);

    connect(ui->tb_chooseKsc, SIGNAL(pressed()), this, SLOT(openKscPathDialog()));
    ui->le_ksc->setText(SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString());

    m_highlightNav = new HighlightNavigator(widget);
    ui->layout_nav->addWidget(m_highlightNav);
    ui->layout_nav->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_highlightNav->setContainer(m_previewContainer);
    m_highlightNav->setPluginCallback(m_pluginCallback);
    m_highlightNav->setHighlightCategory(KAITAI_STRUCT_CATEGORY);
    m_highlightNav->setShouldHighlightSelection(true);
}

void KaitaiStruct::openKscPathDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            tr("Select Kaitai Struct Compiler"),
            SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString(),
            tr("KSC (ksc*, kaitai-struct-compiler*)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->le_ksc->setText(fileName);
    SettingsManager::getInstance().setPrivateSetting(KAITAI_PATH_KEY, fileName);
}

bool KaitaiStruct::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
        return false;
    }
    if (!(pluginState.contains("katai_struct_yaml") && pluginState.value("katai_struct_yaml").isString())) {
        return false;
    }

    return true;
}

bool KaitaiStruct::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    // Set the UI fields based on the plugin state
    ui->te_ksy->setPlainText(pluginState.value("katai_struct_yaml").toString());

    return true;
}

QJsonObject KaitaiStruct::getStateFromUi()
{
    QJsonObject pluginState;
    pluginState.insert("katai_struct_yaml", ui->te_ksy->toPlainText());
    return pluginState;
}

void KaitaiStruct::previewBits(QSharedPointer<BitContainerPreview> container)
{
    m_previewContainer = container;
    if (m_highlightNav) {
        m_highlightNav->setContainer(m_previewContainer);
        if (m_previewContainer.isNull()) {
            m_highlightNav->setTitle("");
        }
        else {
            QString resultLabel = m_previewContainer->info()->metadata(KAITAI_RESULT_LABEL).toString();
            if (resultLabel.size() > 28) {
                resultLabel.truncate(25);
                resultLabel += "...";
            }
            m_highlightNav->setTitle(resultLabel);
        }
    }
}

QSharedPointer<const AnalyzerResult> KaitaiStruct::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &recallablePluginState,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    QMetaObject::invokeMethod(this, "clearOutputText", Qt::QueuedConnection);

    if (!canRecallPluginState(recallablePluginState)) {
        return AnalyzerResult::error("Invalid parameters given to plugin");
    }

    progressTracker->setProgressPercent(2);

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return AnalyzerResult::error("Could not allocate a temporary directory");
    }

    QFile inputBitFile(dir.filePath("input_bit_container.bits"));
    QFile outputRangeFile(dir.filePath("parsed_ranges.json"));
    if (!inputBitFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return AnalyzerResult::error("Could not allocate a temporary directory");
    }
    container->bits()->writeTo(&inputBitFile);
    inputBitFile.close();

    progressTracker->setProgressPercent(10);

    if (ui->tabExecOptions->currentWidget() == ui->tab_custom) {
        QString kscPath = SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString();
        if (kscPath.isEmpty()) {
            return AnalyzerResult::error("A Kaitai Struct Compiler path must be specified");
        }

        QFile errorFile(dir.filePath("error.log"));
        QFile stdoutFile(dir.filePath("stdout.log"));

        QFile ksy(dir.filePath("custom.ksy"));
        if (!ksy.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            return AnalyzerResult::error("Could not open ksy file for writing");
        }
        ksy.write(recallablePluginState.value("katai_struct_yaml").toString().toLocal8Bit());
        ksy.close();

        progressTracker->setProgressPercent(20);

    #ifdef Q_OS_WIN
        QStringList kscAgs = {"/C", kscPath, "--debug", "-t", "python", ksy.fileName()};
    #else
        QStringList kscAgs = {"--debug", "-t", "python", ksy.fileName()};
    #endif
        QProcess kscProcess;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QProcessEnvironment envUpdate;
        envUpdate.insert("PATH", env.value("PATH"));
        kscProcess.setProcessEnvironment(envUpdate);
        kscProcess.setWorkingDirectory(dir.path());
        kscProcess.setStandardErrorFile(errorFile.fileName());
        kscProcess.setStandardOutputFile(stdoutFile.fileName());
    #ifdef Q_OS_WIN
        kscProcess.start("cmd.exe", kscAgs);
    #else
        kscProcess.start(kscPath, kscAgs);
    #endif

        kscProcess.waitForFinished();

        if (errorFile.open(QIODevice::ReadOnly)) {
            QString output = errorFile.readAll();
            if (!output.isEmpty()) {
                QMetaObject::invokeMethod(
                        this,
                        "updateOutputText",
                        Qt::QueuedConnection,
                        Q_ARG(QString, "kaitai-stuct-compiler stderr:\n" + output + "\n\n"));
            }
            errorFile.close();
        }

        if (stdoutFile.open(QIODevice::ReadOnly)) {
            QString output = stdoutFile.readAll();
            if (!output.isEmpty()) {
                QMetaObject::invokeMethod(
                        this,
                        "updateOutputText",
                        Qt::QueuedConnection,
                        Q_ARG(QString, "kaitai-stuct-compiler stdout:\n" + output + "\n\n"));
            }
            stdoutFile.close();
        }
    }
    else {
        if (m_selectedPrecompiledFile.isEmpty()) {
            return AnalyzerResult::error("A precompiled struct must be specified");
        }
        QFileInfo info(m_selectedPrecompiledFile);
        QFile::copy(m_selectedPrecompiledFile, dir.filePath(info.fileName()));
    }

    progressTracker->setProgressPercent(40);


    QString pyDepDir = dir.filePath("pydeps");
    HobbitsPython::recursiveDirCopy(":/kaitaistruct/scripts/dependencies", pyDepDir);

    auto pyRequest = PythonRequest::create(":/kaitaistruct/scripts/runner.py")->setFunctionName("parse_data");
    pyRequest->addPathExtension(pyDepDir);
    pyRequest->addArg(PythonArg::qString(inputBitFile.fileName()));
    pyRequest->addArg(PythonArg::qString(outputRangeFile.fileName()));
    auto watcher = HobbitsPython::getInstance().runProcessScript(pyRequest, true);
    watcher->watcher()->future().waitForFinished();
    auto result = watcher->result();

    QString output = "";
    if (!result->getStdOut().isEmpty()) {
        output += "Python stdout:\n" + result->getStdOut() + "\n\n";
    }
    if (!result->getStdErr().isEmpty()) {
        output += "Python stderr:\n" + result->getStdErr() + "\n\n";
    }
    if (!result->errors().isEmpty()) {
        output += "Other errors:\n" + result->errors().join("\n") + "\n\n";
    }
    if (!output.isEmpty()) {
        QMetaObject::invokeMethod(
                this,
                "updateOutputText",
                Qt::QueuedConnection,
                Q_ARG(QString, output));
    }

    progressTracker->setProgressPercent(85);

    if (!outputRangeFile.exists()) {
        QString errorString = "No analysis file was produced - check the Output tab to see if ksc or python produced any errors.";
        errorString += "\n\nCommon problems to check for:";
        errorString += "\n- Are you pointing to a valid Python 3 with the kaitaistruct package 0.9 installed?";
        errorString += "\n- Are you pointing to a valid kaitai-struct-compiler version 0.9?";
        return AnalyzerResult::error(errorString);
    }

    if (!outputRangeFile.open(QIODevice::ReadOnly)) {
        return AnalyzerResult::error("Could not open analysis file for reading");
    }
    QJsonDocument outputData = QJsonDocument::fromJson((outputRangeFile.readAll()));
    outputRangeFile.close();
    if (!outputData.isObject()) {
        return AnalyzerResult::error("Output analysis file has an invalid format");
    }
    QJsonObject outputObj = outputData.object();
    if (!outputObj.contains("sections") || !outputObj.value("sections").isArray()) {
        return AnalyzerResult::error("Output analysis file doesn't contain a 'sections' specification");
    }
    QList<RangeHighlight> highlights;
    m_highlightColorIdx = 0;
    QMap<QString, QPair<Range, QList<QString>>> labelMap;
    QList<QString> topLevel;
    QJsonArray sections = outputObj.value("sections").toArray();
    int sectionNum = 0;
    for (auto section: sections) {
        if (!section.isObject()) {
            continue;
        }
        sectionNum++;
        QJsonObject s = section.toObject();
        QString label = QString("<%1>").arg(sectionNum);
        if (s.contains("label") && s.value("label").isString()) {
            label = s.value("label").toString();
        }
        if (!s.contains("start") || !s.contains("end") || !s.value("start").isDouble() || !s.value("end").isDouble()) {
            labelMap.insert(label, {Range(), {}});
        }
        else {
            Range range(qint64(s.value("start").toDouble())*8, qint64(s.value("end").toDouble())*8 - 1);
            labelMap.insert(label, {range, {}});
        }

        if (!s.contains("parent") || s.value("parent").toString().isEmpty()) {
            topLevel.append(label);
        }
        else if (labelMap.contains(s.value("parent").toString())) {
            labelMap[s.value("parent").toString()].second.append(label);
        }
    }

    for (auto label : topLevel) {
        highlights.append(makeHighlight(label, labelMap));
    }

    QSharedPointer<BitInfo> bitInfo = BitInfo::copyFromContainer(container);
    bitInfo->addHighlights(highlights);

    return AnalyzerResult::result(bitInfo, recallablePluginState);
}

RangeHighlight KaitaiStruct::makeHighlight(QString label, const QMap<QString, QPair<Range, QList<QString>>> &rangeData)
{
    QList<QColor> colors = {
        QColor(100, 220, 100, 85),
        QColor(100, 0, 255, 50),
        QColor(0, 150, 230, 100),
        QColor(200, 140, 0, 100),
        QColor(250, 50, 0, 100)
    };
    auto pair = rangeData.value(label);
    if (pair.second.isEmpty()) {
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, pair.first, colors.at(m_highlightColorIdx));
        m_highlightColorIdx = (m_highlightColorIdx + 1) % colors.size();
        return highlight;
    }
    else {
        int parentColorIndex = m_highlightColorIdx;
        m_highlightColorIdx = 0;
        QList<RangeHighlight> children;
        for (auto child : pair.second) {
            children.append(makeHighlight(child, rangeData));
        }
        m_highlightColorIdx = parentColorIndex;
        auto highlight = RangeHighlight(KAITAI_STRUCT_CATEGORY, label, children, colors.at(m_highlightColorIdx));
        m_highlightColorIdx = (m_highlightColorIdx + 1) % colors.size();
        return highlight;
    }
}


void KaitaiStruct::updateOutputText(QString text)
{
    ui->te_output->appendPlainText(text);
    ui->tabWidget->setCurrentIndex(1);
}

void KaitaiStruct::clearOutputText()
{
    ui->te_output->clear();
}
