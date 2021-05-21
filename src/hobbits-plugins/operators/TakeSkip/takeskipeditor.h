#ifndef TAKESKIPEDITOR_H
#define TAKESKIPEDITOR_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class TakeSkipEditor;
}

class TakeSkipEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    TakeSkipEditor(QSharedPointer<ParameterDelegate> delegate);
    ~TakeSkipEditor() override;

    QString title() override;

    virtual bool setParameters(const Parameters &parameters) override;
    virtual Parameters parameters() override;

private slots:
    void showHelp();
    void requestRun();
    void interleaveSelectionChanged();

private:
    Ui::TakeSkipEditor *ui;
    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<ParameterHelper> m_stateHelper;
};

#endif // TAKESKIPEDITOR_H
