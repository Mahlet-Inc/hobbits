#ifndef %{JS: '%{ImportFormClassName}'.toUpperCase()}_H
#define %{JS: '%{ImportFormClassName}'.toUpperCase()}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class %{ImportFormClassName};
}

class %{ImportFormClassName} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    %{ImportFormClassName}(QSharedPointer<ParameterDelegate> delegate);
    ~%{ImportFormClassName}() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::%{ImportFormClassName} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // %{JS: '%{ImportFormClassName}'.toUpperCase()}_H