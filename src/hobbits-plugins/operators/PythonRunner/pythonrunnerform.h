#ifndef PYTHONRUNNERFORM_H
#define PYTHONRUNNERFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class PythonRunnerForm;
}

class PythonRunnerForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    PythonRunnerForm(QSharedPointer<ParameterDelegate> delegate);
    ~PythonRunnerForm() override;

    QString title() override;

    virtual bool setParameters(const Parameters &parameters) override;
    virtual Parameters parameters() override;


public Q_SLOTS:
    void openHelpDialog();
    void updateOutputText(QString);
    void clearOutputText();

private:
    Ui::PythonRunnerForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
    QString m_outputText;
};

#endif // PYTHONRUNNERFORM_H
