#ifndef DIGRAPHPLOTFORM_H
#define DIGRAPHPLOTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class DigraphPlotForm;
}

class DigraphPlotForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    DigraphPlotForm(QSharedPointer<ParameterDelegate> delegate);
    ~DigraphPlotForm() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    Ui::DigraphPlotForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // DIGRAPHPLOTFORM_H
