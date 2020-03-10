#ifndef HEADERFRAMER_H
#define HEADERFRAMER_H

#include "mathparser.h"
#include "operatorinterface.h"


namespace Ui
{
class HeaderFramer;

}

class HeaderFramer : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.HeaderFramer")
    Q_INTERFACES(OperatorInterface)

public:
    HeaderFramer();

    QString getName() override;

    OperatorInterface* createDefaultOperator() override;
    void applyToWidget(QWidget *widget) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

private slots:
    void fixedLengthToggled(int fixed);
    void validateHeader(QString header);
    void addHeader();
    void checkSelectedHeader();
    void removeHeader();

private:
    Ui::HeaderFramer *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;

    QString getHeaderString();

};

#endif // HEADERFRAMER_H
