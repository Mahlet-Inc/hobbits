#ifndef METADATAFORM_H
#define METADATAFORM_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class MetadataForm;
}

class MetadataForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    MetadataForm(QSharedPointer<ParameterDelegate> delegate);
    ~MetadataForm() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::MetadataForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // METADATAFORM_H
