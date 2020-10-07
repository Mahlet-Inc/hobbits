#ifndef HIGHLIGHTNAVIGATOR_H
#define HIGHLIGHTNAVIGATOR_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "displayhandle.h"
#include "bitcontainerpreview.h"
#include "hobbits-widgets_global.h"

namespace Ui {
class HighlightNavigator;
}

class HOBBITSWIDGETSSHARED_EXPORT HighlightNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit HighlightNavigator(QWidget *parent = nullptr);
    ~HighlightNavigator();

    bool selectRow(QString);
    QString currentlySelectedLabel();

public slots:
    void setPluginCallback(QSharedPointer<DisplayHandle> displayHandle);
    void setContainer(QSharedPointer<BitContainerPreview> container);
    void setHighlightCategory(QString category);
    void setTitle(QString title);
    void refresh();
    void selectNext();
    void selectPrevious();
    void updateSelection();
    void setShouldHighlightSelection(bool);

signals:
    void selectionChanged();

private:
    QTreeWidgetItem* highlightToItem(const RangeHighlight &highlight, int &count) const;

    Ui::HighlightNavigator *ui;
    QList<RangeHighlight> m_highlights;
    int m_allHighlightCount;
    QString m_category;
    QSharedPointer<BitContainerPreview> m_container;
    QSharedPointer<DisplayHandle> m_displayHandle;
    bool m_shouldHighlightSelection;
};

#endif // HIGHLIGHTNAVIGATOR_H
