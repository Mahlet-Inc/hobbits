#ifndef {{cookiecutter.import_editor|upper}}_H
#define {{cookiecutter.import_editor|upper}}_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class {{cookiecutter.import_editor}};
}

class {{cookiecutter.import_editor}} : public AbstractParameterEditor
{
    Q_OBJECT

public:
    {{cookiecutter.import_editor}}(QSharedPointer<ParameterDelegate> delegate);
    ~{{cookiecutter.import_editor}}() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::{{cookiecutter.import_editor}} *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // {{cookiecutter.import_editor|upper}}_H
