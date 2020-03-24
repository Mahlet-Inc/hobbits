#ifndef BITCONTAINERPREVIEW_H
#define BITCONTAINERPREVIEW_H

#include "bitcontainer.h"
#include <QObject>

#include "hobbits-core_global.h"

/// A proxy class for BitContainers that prevents bit editing, but allows limited metadata editing
class HOBBITSCORESHARED_EXPORT BitContainerPreview : public QObject
{
    Q_OBJECT

public:
    explicit BitContainerPreview(QSharedPointer<BitContainer> m_bitContainer);

    QString getName() const;
    QSharedPointer<const BitArray> bits() const;
    QSharedPointer<const BitInfo> bitInfo() const;

    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

private:
    QSharedPointer<BitContainer> m_bitContainer;
};

#endif // BITCONTAINERPREVIEW_H
