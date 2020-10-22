#include "parameterdelegateui.h"


ParameterDelegateUi::ParameterDelegateUi(
        QList<ParameterDelegate::ParameterInfo> parameterInfos,
        std::function<QString(const QJsonObject&)> actionDescriber,
        std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator) :
    ParameterDelegate(parameterInfos, actionDescriber, editorCreator)
{

}

QSharedPointer<ParameterDelegateUi> ParameterDelegateUi::create(
        QList<ParameterDelegate::ParameterInfo> parameterInfos,
        std::function<QString (const QJsonObject &)> actionDescriber,
        std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator)
{
    return QSharedPointer<ParameterDelegateUi>(new ParameterDelegateUi(parameterInfos, actionDescriber, editorCreator));
}
