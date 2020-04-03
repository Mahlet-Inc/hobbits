#ifndef HIGHLIGHTNAVIGATOR_H
#define HIGHLIGHTNAVIGATOR_H

#include <QWidget>
#include "bitcontainerpreview.h"
#include "plugincallback.h"

namespace Ui {
class HighlightNavigator;
}

class HighlightNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit HighlightNavigator(QWidget *parent = nullptr);
    ~HighlightNavigator();

    int currentlySelectedRow();
    bool selectRow(QString);
    QString currentlySelectedLabel();

public slots:
    void setPluginCallback(QSharedPointer<PluginCallback> pluginCallback);
    void setContainer(QSharedPointer<BitContainerPreview> container);
    void setHighlightCategory(QString category);
    void setTitle(QString title);
    void refresh();
    void selectNext();
    void selectPrevious();
    void selectRow(int);
    void updateSelection();
    void setShouldHighlightSelection(bool);

signals:
    void selectionChanged();

private:
    Ui::HighlightNavigator *ui;
    QList<RangeHighlight> m_highlights;
    QString m_category;
    QSharedPointer<BitContainerPreview> m_container;
    QSharedPointer<PluginCallback> m_pluginCallback;
    bool m_shouldHighlightSelection;
};

#endif // HIGHLIGHTNAVIGATOR_H
