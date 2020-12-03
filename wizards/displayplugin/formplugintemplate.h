#ifndef %{JS: '%{FormClassName}'.toUpperCase()}_H
#define %{JS: '%{FormClassName}'.toUpperCase()}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class %{FormClassName};
}

class %{FormClassName} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    %{FormClassName}(QSharedPointer<ParameterDelegate> delegate);
    ~%{FormClassName}() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::%{FormClassName} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // %{JS: '%{FormClassName}'.toUpperCase()}_H