#ifndef PRBSGENERATORFORM_H
#define PRBSGENERATORFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class LfsrForm;
}

class LfsrForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    LfsrForm(QSharedPointer<ParameterDelegate> delegate);
    ~LfsrForm() override;

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

public slots:
    void updateEndState(QString lfsr);
    void showHelp();

private:
    Ui::LfsrForm *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // PRBSGENERATORFORM_H
