#ifndef BYTERASTERCONTROLS_H
#define BYTERASTERCONTROLS_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class ByteRasterControls;
}

class ByteRasterControls : public AbstractParameterEditor
{
    Q_OBJECT

public:
    ByteRasterControls(QSharedPointer<ParameterDelegate> delegate);

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    Ui::ByteRasterControls *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // BYTERASTERCONTROLS_H
