#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bitcontainermanagerui.h"
#include "displayinterface.h"
#include "operatorinterface.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "hobbitspluginmanager.h"
#include "previewscrollbar.h"
#include "batcheditor.h"
#include "multidisplaywidget.h"

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

    class PluginProgress {
    public:
        QUuid id;
        QProgressBar *progressBar;
        QPushButton *cancelButton;
    };

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
public slots:
    void on_pb_operate_clicked();
    void on_pb_analyze_clicked();

    void loadPlugins();

    void importBitfile(QString file);
    void importBytes(QByteArray rawBytes, QString name);

    void checkOperatorInput(QString pluginName = "");
    void checkCurrentDisplays();

    void activateBitContainer(QSharedPointer<BitContainer> selected, QSharedPointer<BitContainer> deselected);
    void currBitContainerChanged();
    void setCurrentBitContainer();
    void deleteCurrentBitcontainer();
    void deleteAllBitContainers();

    void setStatus(QString status);

    void requestAnalyzerRun(QString pluginName, const Parameters &parameters);
    void requestOperatorRun(QString pluginName, const Parameters &parameters);
    void requestImportRun(QString pluginName, const Parameters &parameters);
    void requestExportRun(QString pluginName, const Parameters &parameters);

    QSharedPointer<BitContainer> currContainer();

    void applyBatchFile(QString fileName);

    void warningMessage(QString message, QString windowTitle = "Warning");

private slots:
    void on_action_Apply_Batch_triggered();
    void on_action_Save_Batch_triggered();
    void on_action_About_triggered();
    void on_actionPreferences_triggered();

    void pluginActionStarted(QUuid);
    void pluginActionFinished(QUuid);
    void pluginActionProgress(QUuid, int);

    void initializeDisplays();
    void addDisplayGroup();
    void removeDisplayGroup(int idx);
    void initializeImporterExporters();

    void populateRecentExportsMenu(QPair<QString, Parameters> addition = QPair<QString, Parameters>(), QPair<QString, Parameters> removal = QPair<QString, Parameters>());
    void populateRecentImportsMenu(QPair<QString, Parameters> addition = QPair<QString, Parameters>(), QPair<QString, Parameters> removal = QPair<QString, Parameters>());
    void populatePluginActionMenu(QString key, QMenu* menu,
                                  const std::function<QString(QString, Parameters)> getLabel,
                                  const std::function<void(QString, Parameters)> triggerAction,
                                  QPair<QString, Parameters> addition = QPair<QString, Parameters>(),
                                  QPair<QString, Parameters> removal = QPair<QString, Parameters>());
    void populateRecentBatchesMenu(QString addition = QString(), QString removal = QString());

    void setupSplitViewMenu();

    void sendBitContainerPreview();
    static void processBitPreview(QSharedPointer<BitContainerPreview> preview, AbstractParameterEditor*  editor);


    void on_action_BatchEditor_triggered();

private:
    Ui::MainWindow *ui;

    QString m_extraPluginPath;

    QSharedPointer<BitContainerManagerUi> m_bitContainerManager;
    QSharedPointer<HobbitsPluginManager> m_pluginManager;
    QSharedPointer<PluginActionManager> m_pluginActionManager;

    QHash<QUuid, PluginProgress*> m_pluginProgress;
    QHash<QUuid, QSharedPointer<ImporterRunner>> m_pendingImports;
    QHash<QUuid, QSharedPointer<ExporterRunner>> m_pendingExports;

    QMap<int, QSharedPointer<OperatorInterface>> m_operatorMap;
    QMap<QSharedPointer<OperatorInterface>, AbstractParameterEditor*> m_operatorUiMap;
    QMap<int, QSharedPointer<AnalyzerInterface>> m_analyzerMap;
    QMap<QSharedPointer<AnalyzerInterface>, AbstractParameterEditor*> m_analyzerUiMap;

    QList<MultiDisplayWidget*> m_displayWidgets;
    QSplitter *m_displayTabsSplitter;
    QSharedPointer<DisplayHandle> m_displayHandle;
    QList<QWidget*> m_currControlWidgets;
    PreviewScrollBar *m_previewScroll;

    BatchEditor *m_batchEditor;

    QMenu *m_splitViewMenu;
};

#endif // MAINWINDOW_H
