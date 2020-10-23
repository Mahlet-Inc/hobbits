#ifndef BITCONTAINERPREVIEW_H
#define BITCONTAINERPREVIEW_H

#include "bitcontainer.h"
#include <QObject>
#include "hobbits-core_global.h"

/**
  * @brief The BitContainerPreview class is a proxy class for BitContainers that prevents bit editing, but allows limited metadata editing
  *
  * \see BitContainer
*/
class HOBBITSCORESHARED_EXPORT BitContainerPreview : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<BitContainerPreview> wrap(QSharedPointer<BitContainer> bitContainer);

    QString name() const;
    QSharedPointer<const BitArray> bits() const;
    QSharedPointer<const BitInfo> info() const;

    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

Q_SIGNALS:
    void changed();

private:
    BitContainerPreview(QSharedPointer<BitContainer> bitContainer);
    QSharedPointer<BitContainer> m_bitContainer;
};

#endif // BITCONTAINERPREVIEW_H
