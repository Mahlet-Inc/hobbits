#ifndef MULTIDISPLAYWIDGET_H
#define MULTIDISPLAYWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include "displaywidget.h"
#include "hobbitspluginmanager.h"

class MultiDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiDisplayWidget(QSharedPointer<HobbitsPluginManager> pluginManager,
                                QSharedPointer<DisplayHandle> handle,
                                QWidget *parent = nullptr);



    QSharedPointer<DisplayInterface> activeDisplay();
    DisplayWidget* activeDisplayWidget();
    AbstractParameterEditor* createEditorForActiveDisplay();

signals:
    void activeDisplayChanged(QSharedPointer<DisplayInterface> parts);

public slots:

private:
    class DisplayParts {
    public:
        static QSharedPointer<DisplayParts> create(QSharedPointer<DisplayInterface> interface,
                                                   QSharedPointer<DisplayHandle> handle);
        ~DisplayParts();

        QSharedPointer<DisplayInterface> interface;
        DisplayWidget* display;
    };

    QTabWidget* m_tabs;
    QMap<int, QSharedPointer<DisplayParts>> m_displayMap;

};

#endif // MULTIDISPLAYWIDGET_H
