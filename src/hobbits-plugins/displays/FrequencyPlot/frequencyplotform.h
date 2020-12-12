#ifndef FREQUENCYPLOTFORM_H
#define FREQUENCYPLOTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class FrequencyPlotForm;
}

class FrequencyPlotForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    FrequencyPlotForm(QSharedPointer<ParameterDelegate> delegate);
    ~FrequencyPlotForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    Ui::FrequencyPlotForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // FREQUENCYPLOTFORM_H
