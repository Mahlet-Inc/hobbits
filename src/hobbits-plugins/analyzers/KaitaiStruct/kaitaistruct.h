#ifndef KAITAISTRUCT_H
#define KAITAISTRUCT_H

#include "analyzerinterface.h"
#include "parameterdelegate.h"

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
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) override;


private:
    typedef struct KsField {
        QString label;
        Range range;
        QString value;
        QString type;
        QVector<QString> children;
    } KsField;
    RangeHighlight makeHighlight(QString label, const QMap<QString, QSharedPointer<KsField>> &fieldData, int &colorIdx);

    QSharedPointer<ParameterDelegate> m_delegate;

};

#endif // KAITAISTRUCT_H
