#ifndef DOTPLOTFORM_H
#define DOTPLOTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class DotPlotForm;
}

class DotPlotForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    DotPlotForm(QSharedPointer<ParameterDelegate> delegate);
    ~DotPlotForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    Ui::DotPlotForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // DOTPLOTFORM_H
