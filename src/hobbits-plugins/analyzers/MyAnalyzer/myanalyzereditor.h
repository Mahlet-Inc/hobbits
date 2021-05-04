#ifndef MYANALYZEREDITOR_H
#define MYANALYZEREDITOR_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class MyAnalyzerEditor;
}

class MyAnalyzerEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    MyAnalyzerEditor(QSharedPointer<ParameterDelegate> delegate);
    ~MyAnalyzerEditor() override;

    QString title() override;

    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::MyAnalyzerEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // MYANALYZEREDITOR_H
