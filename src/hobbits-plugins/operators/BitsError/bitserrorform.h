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

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

private:
    Ui::BitsErrorForm *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // BITSERRORFORM_H
