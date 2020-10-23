#ifndef SYMBOLREMAPPERFORM_H
#define SYMBOLREMAPPERFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "remapmodel.h"

namespace Ui
{
class SymbolRemapperForm;
}


class SymbolRemapperForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    SymbolRemapperForm(QSharedPointer<ParameterDelegate> delegate);
    ~SymbolRemapperForm() override;

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

private:
    Ui::SymbolRemapperForm *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<ParameterHelper> m_paramHelper;
    QString m_outputText;
    RemapModel* m_remapModel;
};

#endif // SYMBOLREMAPPERFORM_H
