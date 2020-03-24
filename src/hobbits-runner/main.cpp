#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>
#ifdef Q_OS_UNIX
#include <iostream>
#include <unistd.h>
#endif
#include "hobbitsrunnerinfo.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "pluginmanager.h"
#include "settingsmanager.h"
#include "templatefilehandler.h"
#include <QJsonArray>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("Hobbits Runner");
    QCoreApplication::setApplicationVersion(HobbitsRunnerInfo::getRunnerVersion());

    QCommandLineParser parser;
    parser.setApplicationDescription("Command-line interface for bitstream analysis and processing");
    parser.addHelpOption();
    parser.addVersionOption();


    parser.addPositionalArgument(
            "mode",
            "The mode to run in\
'run': applies a given template to input data - requires a template parameter");

    QCommandLineOption inputFileOption(
        QStringList() << "i" << "input",
            QCoreApplication::translate(
                "main",
                "File to open on application startup. Use '-' for piped input. This option can be specified more than once if multiple input files are needed"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(inputFileOption);

    QCommandLineOption templateOption(
        QStringList() << "t" << "template",
            QCoreApplication::translate("main", "Template file to use for processing"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(templateOption);

    QCommandLineOption outputPrefixOption(
        QStringList() << "o" << "output",
            QCoreApplication::translate("main", "A path prefix for output files"),
            QCoreApplication::translate("main", "file prefix"));
    parser.addOption(outputPrefixOption);


    QCommandLineOption extraPluginPathOption(
        QStringList() << "p" << "extra-plugin-path",
            QCoreApplication::translate("main", "An additional path to search for plugins in"),
            QCoreApplication::translate("main", "path"));
    parser.addOption(extraPluginPathOption);


    QCommandLineOption configFilePathOption(
        QStringList() << "c" << "config",
            QCoreApplication::translate("main", "Override the default config file location"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(configFilePathOption);

    parser.process(a);

    QTextStream out(stdout);
    QTextStream err(stderr);
    if (parser.positionalArguments().size() != 1) {
        err << "Error: Cannot run without specifying a mode" << endl;
        err << parser.helpText() << endl;
        return -1;
    }

    QByteArray pipedInData;
#ifdef Q_OS_UNIX
    if (parser.isSet(inputFileOption) && parser.value(inputFileOption) == "-") {
        while (!std::cin.eof()) {
            char arr[1024];
            std::cin.read(arr, sizeof(arr));
            int s = std::cin.gcount();
            pipedInData.append(arr, s);
        }
    }
#endif


    if (parser.isSet(configFilePathOption)) {
        SettingsManager::getInstance().setConfigFilePath(parser.value(configFilePathOption));
    }

    // Initialize some stuff
    QSharedPointer<PluginManager> pluginManager = QSharedPointer<PluginManager>(new PluginManager);
    QSharedPointer<PluginActionManager> pluginActionManager = QSharedPointer<PluginActionManager>(
            new PluginActionManager(
                    pluginManager));

    // Load plugins
    QStringList pluginPaths;
    QStringList warnings;
    if (parser.isSet(extraPluginPathOption)) {
        pluginPaths.append(parser.value(extraPluginPathOption).split(":"));
    }
    pluginPaths.append(
            SettingsManager::getInstance().getPluginLoaderSetting(
                    SettingsData::PLUGIN_PATH_KEY).toString().split(":"));
    for (QString pluginPath : pluginPaths) {
        if (pluginPath.startsWith("~/")) {
            pluginPath.replace(0, 1, QDir::homePath());
        }
        else if (!pluginPath.startsWith("/")) {
            pluginPath = a.applicationDirPath() + "/" + pluginPath;
        }
        warnings.append(pluginManager->loadPlugins(pluginPath));
    }
    for (auto warning : warnings) {
        err << "Plugin load warning: " << warning << endl;
    }

    // Run
    QString mode = parser.positionalArguments().at(0);
    if (mode == "run") {
        if (!parser.isSet(templateOption) || !parser.isSet(inputFileOption)) {
            err << "Error: Cannot run in 'run' mode without a template and input specified" << endl;
            err << parser.helpText() << endl;
            return -1;
        }
        QList<QSharedPointer<BitContainer>> targetContainers;
        QByteArray inputData;
        if (pipedInData.isNull()) {
            for (QString fileName : parser.values(inputFileOption)) {
                QFile inputFile(fileName);
                if (!inputFile.open(QIODevice::ReadOnly)) {
                    err << "Error: cannot open input file: " << parser.value(inputFileOption) << endl;
                    return -1;
                }
                auto container = QSharedPointer<BitContainer>(new BitContainer());
                container->setBits(&inputFile);
                targetContainers.append(container);
                inputFile.close();
            }
        }
        else {
            auto container = QSharedPointer<BitContainer>(new BitContainer());
            container->setBits(pipedInData);
            targetContainers.append(container);
        }

        QSharedPointer<BitContainerManager> bitManager = QSharedPointer<BitContainerManager>(new BitContainerManager());
        for (auto container : targetContainers) {
            bitManager->getTreeModel()->addContainer(container);
        }

        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::reportError,
                [&err, &a, pluginActionManager](QString error) {
            err << "Plugin Action Error: " << error;
            pluginActionManager->cancelAction();
            a.exit(-1);
        });

        QString outputPrefix = "hobbits_output_";
        if (parser.isSet(outputPrefixOption)) {
            outputPrefix = parser.value(outputPrefixOption);
        }

        int outputNumber = 1;
        QObject::connect(
                bitManager.data(),
                &BitContainerManager::currSelectionChanged,
                [&a, bitManager, &outputNumber, pluginActionManager, outputPrefix](const QItemSelection &selected,
                                                                                   const QItemSelection &deselected) {
            Q_UNUSED(selected)
            Q_UNUSED(deselected)
            auto container = bitManager->getCurrentContainer();
            QFile output(QString("%1%2").arg(outputPrefix).arg(outputNumber++));
            output.open(QIODevice::WriteOnly);
            container->bits()->writeTo(&output);
            output.close();
        });

        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::lineageQueueCompleted,
                [&a]() {
            a.exit();
        });

        warnings.clear();
        auto lineageTree = TemplateFileHandler::loadTemplate(parser.value(templateOption), &warnings);
        if (lineageTree.isNull()) {
            err << warnings.join("\n");
            return -1;
        }
        else if (!warnings.isEmpty()) {
            err << warnings.join("\n");
        }
        QList<QUuid> additionalInputs;
        for (int i = 1; i < targetContainers.size(); i++) {
            additionalInputs.append(targetContainers.at(i)->getId());
        }
        TemplateFileHandler::applyLineageTree(
                targetContainers.at(0)->getId(),
                additionalInputs,
                lineageTree,
                QFileInfo(parser.value(templateOption)).fileName().section(".", 0, 0),
                bitManager,
                pluginActionManager);
        a.exec();
    }
    else {
        err << QString("Error: Cannot run with mode option '%1'").arg(mode) << endl;
        err << parser.helpText() << endl;
        return -1;
    }
}
