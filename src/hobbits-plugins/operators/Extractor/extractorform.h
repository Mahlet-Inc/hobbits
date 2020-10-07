#ifndef EXTRACTORFORM_H
#define EXTRACTORFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "highlightnavigator.h"

namespace Ui
{
class ExtractorForm;
}


class ExtractorForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    ExtractorForm(QSharedPointer<ParameterDelegate> delegate);
    ~ExtractorForm() override;

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

    virtual void previewBits(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;

private slots:
    void setHighlightCategory(QString);

private:
    Ui::ExtractorForm *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<ParameterHelper> m_stateHelper;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<BitContainerPreview> m_previewContainer;
};

#endif // EXTRACTORFORM_H
