#ifndef SIMPLEPARAMETEREDITOR_H
#define SIMPLEPARAMETEREDITOR_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "hobbits-widgets_global.h"

namespace Ui {
class SimpleParameterEditor;
}

class HOBBITSWIDGETSSHARED_EXPORT SimpleParameterEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit SimpleParameterEditor(QSharedPointer<ParameterDelegate> delegate, QString title);
    ~SimpleParameterEditor() override;

    QString title() override;

    virtual bool setParameters(QJsonObject parameters) override;
    virtual QJsonObject parameters() override;

private:
    Ui::SimpleParameterEditor *ui;
    QSharedPointer<ParameterHelper> m_stateHelper;
    QString m_title;
};

#endif // SIMPLEPARAMETEREDITOR_H
