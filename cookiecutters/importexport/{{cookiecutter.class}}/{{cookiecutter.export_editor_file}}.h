#ifndef {{cookiecutter.export_editor|upper}}_H
#define {{cookiecutter.export_editor|upper}}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class {{cookiecutter.export_editor}};
}

class {{cookiecutter.export_editor}} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    {{cookiecutter.export_editor}}(QSharedPointer<ParameterDelegate> delegate);
    ~{{cookiecutter.export_editor}}() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::{{cookiecutter.export_editor}} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // {{cookiecutter.export_editor|upper}}_H
