#ifndef PARAMETERDELEGATEUI_H
#define PARAMETERDELEGATEUI_H

#include "parameterdelegate.h"
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT ParameterDelegateUi : public ParameterDelegate
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
private:
};

#endif // PARAMETERDELEGATEUI_H
