#pragma once

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class USBDeviceImportEditor;
}

class USBDeviceImportEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    USBDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate);
    ~USBDeviceImportEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::USBDeviceImportEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

