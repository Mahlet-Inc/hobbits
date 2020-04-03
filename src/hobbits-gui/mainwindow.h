#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bitcontainermanager.h"
#include "displayinterface.h"
#include "operatorinterface.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "pluginmanager.h"

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString extraPluginPath, QString configFilePath, QWidget *parent = nullptr);

    ~MainWindow() override;

    QSharedPointer<OperatorInterface> getCurrentOperator();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
public slots:
    void on_pb_operate_clicked();
    void on_pb_analyze_clicked();

    void loadPlugins();

    void importBitfile(QString file);
    void importBytes(QByteArray rawBytes, QString name);

    QStringList openHobbitsBits(QString fileName);

    void checkOperatorInput(QString pluginName = "");
    void checkCurrentDisplays();

    void activateBitContainer(QSharedPointer<BitContainer> selected, QSharedPointer<BitContainer> deselected);
    void currBitContainerChanged();
    void setCurrentBitContainer();
    void deleteCurrentBitcontainer();

    void setHoverBit(bool hovering, int bitOffset, int frameOffset);

    void requestAnalyzerRun(QString pluginName, QJsonObject pluginState);
    void requestOperatorRun(QString pluginName, QJsonObject pluginState);
    void requestImportRun(QString pluginName, QJsonObject pluginState = QJsonObject());
    void requestExportRun(QString pluginName, QJsonObject pluginState = QJsonObject());

    QSharedPointer<BitContainer> currContainer();

    void applyTemplateToCurrentContainer(QString fileName);

    void warningMessage(QString message, QString windowTitle = "Warning");

private slots:
    void on_actionOpen_Container_triggered();
    void on_action_Save_Current_Container_triggered();
    void on_action_Export_Template_triggered();
    void on_actionApply_Template_triggered();
    void on_action_About_triggered();
    void on_actionPreferences_triggered();
    void on_tb_scrollReset_clicked();

    void pluginActionStarted();
    void pluginActionFinished();
    void pluginActionProgress(int);

    void initializeDisplays();
    void addDisplayGroup();
    void removeDisplayGroup(int idx);
    void initializeImporterExporters();

    void populateRecentImportsMenu(QPair<QString, QJsonObject> addition = QPair<QString, QJsonObject>(), QPair<QString, QJsonObject> removal = QPair<QString, QJsonObject>());
    void populateRecentTemplatesMenu(QString addition = QString(), QString removal = QString());

    void setupSplitViewMenu();

    void sendBitContainerPreview();

private:
    Ui::MainWindow *ui;

    QString m_extraPluginPath;

    QSharedPointer<BitContainerManager> m_bitContainerManager;
    QSharedPointer<PluginManager> m_pluginManager;
    QSharedPointer<PluginActionManager> m_pluginActionManager;

    QMutex m_previewMutex;
    QSharedPointer<PluginCallback> m_pluginCallback;

    QProgressBar *m_pluginActionProgress;
    QPushButton *m_pluginActionCancel;

    QMap<int, QSharedPointer<OperatorInterface>> m_operatorMap;
    QMap<int, QSharedPointer<AnalyzerInterface>> m_analyzerMap;

    QList<QPair<QMap<int, QSharedPointer<DisplayInterface>>, QTabWidget*>> m_displayMaps;
    QSplitter *m_displayTabsSplitter;
    QSharedPointer<DisplayHandle> m_displayHandle;
    QList<QWidget*> m_currControlWidgets;

    QMenu *m_splitViewMenu;
};

#endif // MAINWINDOW_H
