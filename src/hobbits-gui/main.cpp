#include <hobbitsguiconfig.h>
#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QStyle>
#include "settingsmanager.h"
#include "hobbitsstyle.h"
#ifdef Q_OS_UNIX
#include <iostream>
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HobbitsStyle::applyStyle(a);

    QCoreApplication::setOrganizationName("Mahlet");
    QCoreApplication::setApplicationName("hobbits");
    QCoreApplication::setApplicationVersion(HobbitsGuiConfig::VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("GUI for bitstream analysis and visualization");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", "Files to open on application startup", "[files...]");

    QCommandLineOption inputFileOption(
        QStringList() << "i" << "input",
            QCoreApplication::translate("main", "File to open on application startup (use '-' for piped input)"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(inputFileOption);

    QCommandLineOption batchFileOption(
        QStringList() << "b" << "batch",
            QCoreApplication::translate("main", "Batch file to immediately run on application startup"),
            QCoreApplication::translate("main", "file"));
    parser.addOption(batchFileOption);

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
    QCommandLineOption pythonHomeOption(
        QStringList() << "python-home",
            QCoreApplication::translate("main", "Override the Python installation used for plugins (must match the linked Python version)"),
            QCoreApplication::translate("main", "path"));
    parser.addOption(pythonHomeOption);
#endif

    parser.process(a);

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

    QString extraPluginPath;
    QString configFilePath;

    if (parser.isSet(extraPluginPathOption)) {
        extraPluginPath = parser.value(extraPluginPathOption);
    }

    if (parser.isSet(configFilePathOption)) {
        configFilePath = parser.value(configFilePathOption);
    }

#if HOBBITS_WITH_PYTHON
    if (parser.isSet(pythonHomeOption)) {
        SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, parser.value(pythonHomeOption));
    }
#endif

    MainWindow w(extraPluginPath, configFilePath);
    w.setWindowIcon(QIcon(":/hobbitsgui/images/icons/HobbitsRingSmall.png"));
    w.show();

    if (!pipedInData.isEmpty()) {
        w.importBytes(pipedInData, "Piped Input");
    }
    QStringList files = parser.values(inputFileOption);
    files.append(parser.positionalArguments());
    for (const QString &file : files) {
        if (file != "-") {
            w.importBitfile(file);
        }
    }

    if (parser.isSet(batchFileOption)) {
        w.applyBatchFile(parser.value(batchFileOption));
    }

    return a.exec();
}
