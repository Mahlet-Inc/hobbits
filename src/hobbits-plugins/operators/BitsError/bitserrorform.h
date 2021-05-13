#ifndef BITSERRORFORM_H
#define BITSERRORFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class BitsErrorForm;
}

class BitsErrorForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    BitsErrorForm(QSharedPointer<ParameterDelegate> delegate);
    ~BitsErrorForm() override;

    QString title() override;

    virtual bool setParameters(const Parameters &parameters) override;
    virtual Parameters parameters() override;

signals:
    void parametersChanged();

private:
    Ui::BitsErrorForm *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // BITSERRORFORM_H
