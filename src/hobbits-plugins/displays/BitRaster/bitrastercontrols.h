#ifndef BITRASTERCONTROLS_H
#define BITRASTERCONTROLS_H

#include <QWidget>
#include "parameterdelegate.h"
#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class BitRasterControls;
}

class BitRasterControls : public AbstractParameterEditor
{
    Q_OBJECT

public:
    BitRasterControls(QSharedPointer<ParameterDelegate> delegate);

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

private:
    Ui::BitRasterControls *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // BITRASTERCONTROLS_H
