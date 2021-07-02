#pragma once



#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class EditEditor;
}

class EditEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    EditEditor(QSharedPointer<ParameterDelegate> delegate);
    ~EditEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::EditEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;

signals:
public slots:

    void setLabelText();
    void changeSliderStart();
    void changeSliderLength();
    void changeSpinBoxStart();
    void changeSpinBoxLength();
    void changeTextBox();
    void toggleInsert();

};



