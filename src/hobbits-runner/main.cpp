#include <QCommandLineParser>
#include <QGuiApplication>
#include <QTextStream>
#ifdef Q_OS_UNIX
#include <iostream>
#include <unistd.h>
#endif
#include <hobbitsrunnerconfig.h>
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "hobbitspluginmanager.h"
#include "settingsmanager.h"
#include <QJsonArray>

#include <QTimer>

#if HOBBITS_WITH_PYTHON
#include <hobbitspythonconfig.h>
#include "pythonpluginconfig.h"
#endif

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QGuiApplication::setApplicationName("Hobbits Runner");
    QGuiApplication::setApplicationVersion(HobbitsRunnerConfig::VERSION);

    QString description = "Command-line interface for bitstream analysis and processing";
#if HOBBITS_WITH_PYTHON
    description += QString(" (Python %1 support)").arg(HobbitsPythonConfig::PYTHON_VERSION);
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription(description);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(
            "mode",
            "'run': apply a batch to input data; 'list': list available plugins");

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

#if HOBBITS_WITH_PYTHON
    QCommandLineOption pythonHomePathOption(
        QStringList() << "python-home",
            QCoreApplication::translate("main", "Override the Python installation used for plugins"),
            QCoreApplication::translate("main", "path"));
    parser.addOption(pythonHomePathOption);
#endif

    parser.process(a);

    QTextStream out(stdout);
    QTextStream err(stderr);
    if (parser.positionalArguments().size() != 1) {
        err << "Error: Cannot run without specifying a mode" << "\n";
        err << parser.helpText() << "\n";
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

#if HOBBITS_WITH_PYTHON
    if (parser.isSet(pythonHomePathOption)) {
        SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, parser.value(pythonHomePathOption));
        err << QString("Setting python home: '%1'").arg(parser.value(pythonHomePathOption)) << "\n";
    }
#endif

    // Initialize some stuff
    QSharedPointer<HobbitsPluginManager> pluginManager = QSharedPointer<HobbitsPluginManager>(new HobbitsPluginManager);
    QSharedPointer<PluginActionManager> pluginActionManager = QSharedPointer<PluginActionManager>(
            new PluginActionManager(
                    pluginManager));


    // Load plugins
    const QStringList pluginPaths = SettingsManager::pluginPaths(parser.value(extraPluginPathOption));
    QStringList warnings;

    for (QString pluginPath : pluginPaths) {
        warnings.append(pluginManager->loadPlugins(pluginPath));
    }

#if HOBBITS_WITH_PYTHON
    for (QString pluginPath : pluginPaths) {
        warnings.append(PythonPluginConfig::loadPythonPlugins(pluginPath, pluginManager, [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                            Q_UNUSED(size)
                            Q_UNUSED(delegate)
                            return nullptr;
        }));
    }
#endif


    for (auto warning : warnings) {
        err << "Plugin load warning: " << warning << "\n";
    }

    // Run
    QString mode = parser.positionalArguments().at(0);
    if (mode == "list") {
        int count = 0;
        const auto printPlugins = [&out, &count](const QString &kind, const auto &plugins) {
            for (const auto &plugin : plugins) {
                out << kind << ": " << plugin->name() << "\n";
                ++count;
            }
        };
        printPlugins("Analyzer", pluginManager->analyzers());
        printPlugins("Operator", pluginManager->operators());
        printPlugins("Display", pluginManager->displays());
        printPlugins("Importer/exporter", pluginManager->importerExporters());
        if (count == 0) {
            err << "No plugins found. Searched: " << pluginPaths.join(QDir::listSeparator()) << "\n";
            return 1;
        }
        return 0;
    }
    if (mode == "run") {
        if (!parser.isSet(batchOption)) {
            err << "Error: Cannot run in 'run' mode without a batch specified" << "\n";
            err << parser.helpText() << "\n";
            return -1;
        }
        QList<QSharedPointer<BitContainer>> targetContainers;
        QByteArray inputData;
        if (pipedInData.isNull()) {
            for (QString fileName : parser.values(inputFileOption)) {
                QFile inputFile(fileName);
                if (!inputFile.open(QIODevice::ReadOnly)) {
                    err << "Error: cannot open input file: " << parser.value(inputFileOption) << "\n";
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

        int resultCode = 0;
        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::reportError,
                [&err, &a, &resultCode, pluginActionManager](QString error) {
            resultCode = 1;
            err << "Plugin Action Error: " << error;
            for (auto id : pluginActionManager->runningBatches().keys()) {
                pluginActionManager->cancelById(id);
            }
            a.exit(resultCode);
        });

        QString outputPrefix = "hobbits_output_";
        if (parser.isSet(outputPrefixOption)) {
            outputPrefix = parser.value(outputPrefixOption);
        }

        int outputNumber = 1;
        QObject::connect(
                bitManager.data(),
                &BitContainerManager::containerAdded,
                [bitManager, &outputNumber, pluginActionManager, outputPrefix](QSharedPointer<BitContainer> container) {
            QFile output(QString("%1%2").arg(outputPrefix).arg(outputNumber++));
            output.open(QIODevice::WriteOnly);
            container->bits()->writeTo(&output);
            output.close();
        });

        QObject::connect(
                pluginActionManager.data(),
                &PluginActionManager::batchFinished,
                [&a, &resultCode](QUuid id) {
            Q_UNUSED(id)
            a.exit(resultCode);
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
        return a.exec();
    }
    else {
        err << QString("Error: Cannot run with mode option '%1'").arg(mode) << "\n";
        err << parser.helpText() << "\n";
        return -1;
    }
}
