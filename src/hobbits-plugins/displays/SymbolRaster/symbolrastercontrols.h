#ifndef SYMBOLRASTERCONTROLS_H
#define SYMBOLRASTERCONTROLS_H

#include "colormapmodel.h"
#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class SymbolRasterControls;
}

class SymbolRasterControls : public AbstractParameterEditor
{
    Q_OBJECT

public:
    SymbolRasterControls(QSharedPointer<ParameterDelegate> delegate);

public slots:
    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    Ui::SymbolRasterControls *ui;
    ColorMapModel *m_colorMapModel;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // SYMBOLRASTERCONTROLS_H
