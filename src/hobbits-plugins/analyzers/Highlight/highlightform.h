#ifndef HIGHLIGHTFORM_H
#define HIGHLIGHTFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class HighlightForm;
}

class HighlightForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    HighlightForm(QSharedPointer<ParameterDelegate> delegate);
    ~HighlightForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:

    Ui::HighlightForm *ui;
    QSharedPointer<ParameterHelper> m_helper;
};

#endif // HIGHLIGHTFORM_H
