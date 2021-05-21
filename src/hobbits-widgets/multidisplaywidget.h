#ifndef MULTIDISPLAYWIDGET_H
#define MULTIDISPLAYWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QSplitter>
#include "displaywidget.h"
#include "hobbitspluginmanager.h"

class HOBBITSWIDGETSSHARED_EXPORT MultiDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiDisplayWidget(QSharedPointer<HobbitsPluginManager> pluginManager,
                                QSharedPointer<DisplayHandle> handle,
                                QWidget *parent = nullptr);
    
    ~MultiDisplayWidget();

    bool setActiveDisplay(QString name);
    QSharedPointer<DisplayInterface> activeDisplay() const;
    DisplayWidget* activeDisplayWidget() const;

    QByteArray saveState() const;
    bool restoreState(QByteArray config);

    void setShowViewSelect(bool show);

public slots:
    void activateCurrentDisplay();

private:
    static QByteArray readStreamBytes(QDataStream &stream);

    class DisplayParts {
    public:
        static QSharedPointer<DisplayParts> create(QSharedPointer<DisplayInterface> interface,
                                                    QSharedPointer<DisplayHandle> handle);
        ~DisplayParts();

        AbstractParameterEditor* createEditor();

        QSharedPointer<DisplayInterface> interface;
        DisplayWidget* display;
    };
    QSharedPointer<DisplayHandle> m_handle;

    QTabWidget* m_tabs;
    QMap<int, QSharedPointer<DisplayParts>> m_displayMap;
    QSplitter* m_splitter;

};

#endif // MULTIDISPLAYWIDGET_H
