#include "%{ControlsHeaderFileName}"
#include "ui_%{ControlsHeaderFileName}"


%{ControlWidgetName}::%{ControlWidgetName}() :
    ui(new Ui::%{ControlWidgetName}())
{
    ui->setupUi(this);
}