#ifndef HEXFORM_H
#define HEXFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class HexForm;
}

class HexForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    HexForm(QSharedPointer<ParameterDelegate> delegate);
    ~HexForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    Ui::HexForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // HEXFORM_H
