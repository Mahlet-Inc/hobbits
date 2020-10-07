#ifndef PARAMETERDELEGATEUI_H
#define PARAMETERDELEGATEUI_H

#include "parameterdelegate.h"

class ParameterDelegateUi : public ParameterDelegate, public QEnableSharedFromThis<ParameterDelegateUi>
{
public:
    ParameterDelegateUi(
            QList<ParameterInfo> parameterInfos,
            std::function<QString(const QJsonObject&)> actionDescriber,
            std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);

    static QSharedPointer<ParameterDelegateUi> create(
            QList<ParameterInfo> parameterInfos,
            std::function<QString(const QJsonObject&)> actionDescriber,
            std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);

    virtual AbstractParameterEditor* createEditor(QSize targetBounds = QSize()) override;

private:
    std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> m_editorCreator;
};

#endif // PARAMETERDELEGATEUI_H
