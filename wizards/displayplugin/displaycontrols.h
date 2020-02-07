#ifndef %{JS: '%{ControlWidgetName}'.toUpperCase()}_H
#define %{JS: '%{ControlWidgetName}'.toUpperCase()}_H

#include <QWidget>

namespace Ui
{
class %{ControlWidgetName};
}

class %{ControlWidgetName} : public QWidget
{
    Q_OBJECT

public:
    %{ControlWidgetName}();

private:
    Ui::%{ControlWidgetName} *ui;
};

#endif // %{JS: '%{ControlWidgetName}'.toUpperCase()}_H
