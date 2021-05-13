#ifndef HILBERTPLOTFORM_H
#define HILBERTPLOTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class HilbertPlotForm;
}

class HilbertPlotForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    HilbertPlotForm(QSharedPointer<ParameterDelegate> delegate);
    ~HilbertPlotForm() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    Ui::HilbertPlotForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // HILBERTPLOTFORM_H
