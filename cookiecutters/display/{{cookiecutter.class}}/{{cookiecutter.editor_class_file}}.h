#ifndef {{cookiecutter.editor_class|upper}}_H
#define {{cookiecutter.editor_class|upper}}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class {{cookiecutter.editor_class}};
}

class {{cookiecutter.editor_class}} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    {{cookiecutter.editor_class}}(QSharedPointer<ParameterDelegate> delegate);
    ~{{cookiecutter.editor_class}}() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::{{cookiecutter.editor_class}} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // {{cookiecutter.editor_class|upper}}_H
