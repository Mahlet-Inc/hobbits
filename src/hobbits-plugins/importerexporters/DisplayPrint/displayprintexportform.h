#ifndef DISPLAYPRINTEXPORTFORM_H
#define DISPLAYPRINTEXPORTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "hobbitspluginmanager.h"

namespace Ui
{
class DisplayPrintExportForm;
}

class DisplayPrintExportForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    DisplayPrintExportForm(QSharedPointer<ParameterDelegate> delegate);
    ~DisplayPrintExportForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

    static QSharedPointer<HobbitsPluginManager> loadUpPluginManager();

private slots:
    void on_tb_selectFile_clicked();
    void displaySelected();

private:
    Ui::DisplayPrintExportForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
    QSharedPointer<HobbitsPluginManager> m_pluginManager;
    AbstractParameterEditor* m_displayEditor;
};

#endif // DISPLAYPRINTEXPORTFORM_H
