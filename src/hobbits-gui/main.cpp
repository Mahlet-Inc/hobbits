#include "hobbitsguiconfig.h"
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

//#include "pffft.h"

int main(int argc, char *argv[])
{
    // See http://www.fftw.org/fftw3_doc/Thread-safety.html
    // fftw_make_planner_thread_safe();

    QApplication a(argc, argv);

    HobbitsStyle::applyStyle(a);

    QCoreApplication::setOrganizationName("Mahlet");
    QCoreApplication::setApplicationName("hobbits");
    QCoreApplication::setApplicationVersion(HobbitsGuiConfig::VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("GUI for bitstream analysis and visualization");
    parser.addHelpOption();
    parser.addVersionOption();

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

    QCommandLineOption pythonHomeOption(
        QStringList() << "python-home",
            QCoreApplication::translate("main", "The python home path (must have binary compatibility with the built-in hobbits python capabilities)"),
            QCoreApplication::translate("main", "path"));
    parser.addOption(pythonHomeOption);

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

    if (parser.isSet(pythonHomeOption)) {
        SettingsManager::setTransientSetting(SettingsManager::PYTHON_HOME_KEY, parser.value(pythonHomeOption));
    }

    MainWindow w(extraPluginPath, configFilePath);
    w.setWindowIcon(QIcon(":/hobbitsgui/images/icons/HobbitsRingSmall.png"));
    w.show();

    if (!pipedInData.isEmpty()) {
        w.importBytes(pipedInData, "Piped Input");
    }
    else {
        if (parser.isSet(inputFileOption)) {
            w.importBitfile(parser.value(inputFileOption));
        }
    }

    if (parser.isSet(batchFileOption)) {
        w.applyBatchFile(parser.value(batchFileOption));
    }

    return a.exec();
}
