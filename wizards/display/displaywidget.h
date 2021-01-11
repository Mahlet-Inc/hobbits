#ifndef %{JS: '%{DisplayWidgetName}'.toUpperCase()}_H
#define %{JS: '%{DisplayWidgetName}'.toUpperCase()}_H

#include "displaybase.h"

class %{DisplayWidgetName} : public DisplayBase
{
    Q_OBJECT

public:
    %{DisplayWidgetName}(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // %{JS: '%{DisplayWidgetName}'.toUpperCase()}_H