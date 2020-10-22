#include <QCommandLineParser>
#include <QApplication>
#include <QTextStream>
#ifdef Q_OS_UNIX
#include <iostream>
#include <unistd.h>
#endif
#include "hobbitsrunnerinfo.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "hobbitspluginmanager.h"
#include "settingsmanager.h"
#include <QJsonArray>
#include "pythonpluginconfig.h"
#include <QTimer>

#ifdef HAS_EMBEDDED_PYTHON
#include "hobbitspython.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Hobbits Runner");
    QCoreApplication::setApplicationVersion(HobbitsRunnerInfo::getRunnerVersion());

    QString description = "Command-line interface for bitstream analysis and processing";
#ifdef HAS_EMBEDDED_PYTHON
    description += QString("(built with embedded python support from %1)").arg(HobbitsPython::pythonVersion());
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription(description);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(
            "mode",
            "The mode to run in\
'run': applies a given batch to input data - requires a batch parameter");

    QCommandLineOption inputFileOption(
        QStringList() << "i" << "input",
            QCoreApplication::translate(
                "main",
                "File to open on application startup. Use '-' for piped input. This option can be specified more than once if multiple input files are needed"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(inputFileOption);

    QCommandLineOption batchOption(
        QStringList() << "b" << "batch",
            QCoreApplication::translate("main", "Batch file to use for processing"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(batchOption);

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

    QCommandLineOption pythonHomePathOption(
        QStringList() << "python-home",
            QCoreApplication::translate("main", "Override the default embedded python location"),
            QCoreApplication::translate("main", "path"));
    parser.addOption(pythonHomePathOption);

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
        SettingsManager::setConfigFilePath(parser.value(configFilePathOption));
    }

    if (parser.isSet(pythonHomePathOption)) {
        SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, parser.value(pythonHomePathOption));
        err << QString("Setting python home: '%1'").arg(parser.value(pythonHomePathOption)) << endl;
    }

    // Initialize some stuff
    QSharedPointer<HobbitsPluginManager> pluginManager = QSharedPointer<HobbitsPluginManager>(new HobbitsPluginManager);
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
            SettingsManager::getPluginLoaderSetting(
                    SettingsManager::PLUGIN_PATH_KEY).toString().split(":"));
    QStringList pathBuffer;
    for (QString pluginPath : pluginPaths) {

        if (pluginPath.startsWith("~/")) {
            pluginPath.replace(0, 1, QDir::homePath());
        }
        else if (!pluginPath.startsWith("/")) {
            pluginPath = a.applicationDirPath() + "/" + pluginPath;
        }
        pathBuffer.append(pluginPath);
    }
    pluginPaths = pathBuffer;

    for (QString pluginPath : pluginPaths) {
        warnings.append(pluginManager->loadPlugins(pluginPath));
    }

#ifdef HAS_EMBEDDED_PYTHON
    for (QString pluginPath : pluginPaths) {
        warnings.append(PythonPluginConfig::loadPythonPlugins(pluginPath, pluginManager, [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                            Q_UNUSED(size)
                            Q_UNUSED(delegate)
                            return nullptr;
        }));
    }
#endif


    for (auto warning : warnings) {
        err << "Plugin load warning: " << warning << endl;
    }

    // Run
    QString mode = parser.positionalArguments().at(0);
    if (mode == "run") {
        if (!parser.isSet(batchOption)) {
            err << "Error: Cannot run in 'run' mode without a batch specified" << endl;
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
                auto container = BitContainer::create(&inputFile);
                targetContainers.append(container);
                inputFile.close();
            }
        }
        else {
            auto container = BitContainer::create(pipedInData);
            targetContainers.append(container);
        }

        QSharedPointer<BitContainerManager> bitManager = QSharedPointer<BitContainerManager>(new BitContainerManager());
        for (auto container : targetContainers) {
            bitManager->addContainer(container);
        }
        pluginActionManager->setContainerManager(bitManager);

        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::reportError,
                [&err, &a, pluginActionManager](QString error) {
            err << "Plugin Action Error: " << error;
            for (auto id : pluginActionManager->runningBatches().keys()) {
                pluginActionManager->cancelById(id);
            }
            a.exit(-1);
        });

        QString outputPrefix = "hobbits_output_";
        if (parser.isSet(outputPrefixOption)) {
            outputPrefix = parser.value(outputPrefixOption);
        }

        int outputNumber = 1;
        QObject::connect(
                bitManager.data(),
                &BitContainerManager::containerAdded,
                [&a, bitManager, &outputNumber, pluginActionManager, outputPrefix](QSharedPointer<BitContainer> container) {
            QFile output(QString("%1%2").arg(outputPrefix).arg(outputNumber++));
            output.open(QIODevice::WriteOnly);
            container->bits()->writeTo(&output);
            output.close();
        });

        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::batchFinished,
                [&a, &err](QUuid id) {
            Q_UNUSED(id)
            a.exit();
        });

        warnings.clear();

        QFile file(parser.value(batchOption));
        if (!file.open(QIODevice::ReadOnly)) {
            err << QString("Could not open hobbits batch file '%1'").arg(parser.value(batchOption));
            return -1;
        }

        auto batch = PluginActionBatch::deserialize(QJsonDocument::fromJson(file.readAll()).object());

        if (batch.isNull()) {
            err << "Failed to load batch file";
            return -1;
        }

        if (batch->getRequiredInputs() > targetContainers.size()) {
            err << QString("Given batch requires %1 inputs and only %2 were provided.").arg(batch->getRequiredInputs()).arg(targetContainers.size());
            return -1;
        }

        // make sure this runs after the event loop starts
        QTimer::singleShot(5, [pluginActionManager, batch, targetContainers](){pluginActionManager->runBatch(batch, targetContainers);});
        a.exec();
    }
    else {
        err << QString("Error: Cannot run with mode option '%1'").arg(mode) << endl;
        err << parser.helpText() << endl;
        return -1;
    }
}
