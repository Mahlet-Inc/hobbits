#ifndef BINARYCONTROLS_H
#define BINARYCONTROLS_H

#include "parameterdelegate.h"
#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class BinaryControls;
}

class BinaryControls : public AbstractParameterEditor
{
    Q_OBJECT

public:
    BinaryControls(QSharedPointer<ParameterDelegate> delegate);

    QString title() override;

    virtual bool setParameters(const Parameters &parameters) override;
    virtual Parameters parameters() override;

private:
    Ui::BinaryControls *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // BINARYCONTROLS_H
