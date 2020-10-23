#ifndef KAITAISTRUCT_H
#define KAITAISTRUCT_H

#include "analyzerinterface.h"
#include "parameterdelegateui.h"

class KaitaiStruct : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.KaitaiStruct")
    Q_INTERFACES(AnalyzerInterface)

public:
    KaitaiStruct();

    AnalyzerInterface* createDefaultAnalyzer() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;


private:
    RangeHighlight makeHighlight(QString label, const QMap<QString, QPair<Range, QList<QString>>> &rangeData, int &colorIdx);

    QSharedPointer<ParameterDelegateUi> m_delegate;

};

#endif // KAITAISTRUCT_H
