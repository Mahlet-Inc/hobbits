#ifndef BATCHEDITOR_H
#define BATCHEDITOR_H

#include <QMainWindow>
#include "hobbitspluginmanager.h"
#include "batcheditscene.h"

namespace Ui {
class BatchEditor;
}

class BatchEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit BatchEditor(QSharedPointer<HobbitsPluginManager> pluginManager, QWidget *parent = nullptr);
    ~BatchEditor();

    void setBatch(QSharedPointer<PluginActionBatch> batch);

private slots:
    void on_action_saveBatchAs_triggered();
    void on_action_openBatch_triggered();
    void on_action_createNewBatch_triggered();

private:
    Ui::BatchEditor *ui;
    QSharedPointer<HobbitsPluginManager> m_pluginManager;
    BatchEditScene *m_editScene;
};

#endif // BATCHEDITOR_H
