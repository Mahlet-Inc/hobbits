#ifndef PLUGINSTATEHELPER_H
#define PLUGINSTATEHELPER_H

#include <QVariant>
#include <QJsonValue>
#include <QSharedPointer>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include "hobbits-widgets_global.h"
#include "parameterdelegate.h"

class HOBBITSWIDGETSSHARED_EXPORT ParameterHelper
{
public:
    ParameterHelper(QSharedPointer<ParameterDelegate> delegate);

    void addParameter(QString name,
                      const std::function<bool(QJsonValue)> setInUi,
                      const std::function<QJsonValue()> getFromUi);

    void addCheckBoxBoolParameter(QString name,
                      QCheckBox* checkbox);

    void addSpinBoxIntParameter(QString name,
                      QSpinBox* spinBox);

    void addSliderIntParameter(QString name,
                      QAbstractSlider* slider);

    void addLineEditStringParameter(QString name,
                      QLineEdit* lineEdit);

    void addTextEditStringParameter(QString name,
                      QPlainTextEdit* textEdit);

    void addComboBoxParameter(QString name,
                              QComboBox* comboBox,
                              int role = Qt::UserRole);

    bool applyParametersToUi(const QJsonObject &parameters);
    QJsonObject getParametersFromUi();

private:
    class ParameterInfoUi
    {
    public:
        ParameterInfoUi(const ParameterDelegate::ParameterInfo &parameterInfo,
                  const std::function<bool(QJsonValue)> setInUi,
                  const std::function<QJsonValue()> getFromUi) :
            setInUi(setInUi),
            getFromUi(getFromUi),
            m_info(parameterInfo)
        {

        }

        std::function<bool(QJsonValue)> setInUi;
        std::function<QJsonValue()> getFromUi;

        ParameterDelegate::ParameterInfo info() const {return m_info;}

    private:
        ParameterDelegate::ParameterInfo  m_info;
    };

    QSharedPointer<ParameterDelegate> m_delegate;
    QList<QSharedPointer<ParameterInfoUi>> m_parameterInfos;
};

#endif // PLUGINSTATEHELPER_H
