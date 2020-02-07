#ifndef %{JS: '%{PluginName}'.toUpperCase()}_H
#define %{JS: '%{PluginName}'.toUpperCase()}_H

#include "displayinterface.h"
#include "%{ControlsHeaderFileName}"
#include "%{WidgetHeaderFileName}"

class %{PluginName} : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.%{PluginName}")
    Q_INTERFACES(DisplayInterface)

public:
    %{PluginName}();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    %{DisplayWidgetName}* m_displayWidget;
    %{ControlWidgetName}* m_controlsWidget;
};

#endif // %{JS: '%{PluginName}'.toUpperCase()}_H
