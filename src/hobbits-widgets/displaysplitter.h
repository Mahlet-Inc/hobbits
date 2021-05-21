#pragma once

#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include "hobbits-widgets_global.h"
#include "hobbitspluginmanager.h"
#include "displayhandle.h"
#include "multidisplaywidget.h"

class HOBBITSWIDGETSSHARED_EXPORT DisplaySplitter : public QWidget
{
    Q_OBJECT

public:
    DisplaySplitter(QSharedPointer<HobbitsPluginManager> pluginManager,
                    QSharedPointer<DisplayHandle> handle,
                    MultiDisplayWidget *nonSplitWidget = nullptr,
                    QWidget *parent = nullptr);

    bool isSplit() const;

    Qt::Orientation splitOrientation() const;

    void split(Qt::Orientation orientation);
    void unSplit(bool keepSecond = false);

    QByteArray getConfig() const;
    bool applyConfig(QByteArray config);

private:
    void leaveEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool m_mousePressing;

    MultiDisplayWidget* takeNonSplitWidget();
    QPair<DisplaySplitter*, DisplaySplitter*> takeSplitWidgets();

    QSharedPointer<HobbitsPluginManager> m_pluginManager;
    QSharedPointer<DisplayHandle> m_handle;

    QVBoxLayout *m_vBox;
    QSplitter *m_splitter;

    MultiDisplayWidget *m_nonSplitWidget;

    Qt::Orientation m_splitOrientation;
};
