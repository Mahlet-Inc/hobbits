#ifndef FINDFORM_H
#define FINDFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "highlightnavigator.h"

const QString FIND_COLOR_KEY = "find_color";
const QString FOUND_HIGHLIGHT = "found_patterns";
const QString FOUND_RESULT_LABEL = "found_result_label";

namespace Ui
{
class FindForm;
}

class FindForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    FindForm(QSharedPointer<ParameterDelegate> delegate);
    ~FindForm() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

    void giveDisplayHandle(QSharedPointer<DisplayHandle> handle) override;

private:
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::FindForm *ui;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // FINDFORM_H
