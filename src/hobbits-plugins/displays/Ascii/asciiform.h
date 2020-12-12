#ifndef ASCIIFORM_H
#define ASCIIFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class AsciiForm;
}

class AsciiForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    AsciiForm(QSharedPointer<ParameterDelegate> delegate);
    ~AsciiForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    Ui::AsciiForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // ASCIIFORM_H
