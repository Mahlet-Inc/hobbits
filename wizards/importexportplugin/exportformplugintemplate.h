#ifndef %{JS: '%{ExportFormClassName}'.toUpperCase()}_H
#define %{JS: '%{ExportFormClassName}'.toUpperCase()}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class %{ExportFormClassName};
}

class %{ExportFormClassName} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    %{ExportFormClassName}(QSharedPointer<ParameterDelegate> delegate);
    ~%{ExportFormClassName}() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::%{ExportFormClassName} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // %{JS: '%{ExportFormClassName}'.toUpperCase()}_H