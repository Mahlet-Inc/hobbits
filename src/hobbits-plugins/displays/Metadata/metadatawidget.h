#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include "displayhandle.h"
#include <QWidget>

namespace Ui
{
class MetadataWidget;
}
class MetadataWidget : public QWidget
{
    Q_OBJECT

public:
    MetadataWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

public slots:
    void checkFocus(QSet<DisplayInterface*>);
    void adjustData();

private:
    Ui::MetadataWidget *ui;
    QSharedPointer<DisplayHandle> m_displayHandle;
    DisplayInterface *m_pluginRef;
};

#endif // METADATAWIDGET_H
