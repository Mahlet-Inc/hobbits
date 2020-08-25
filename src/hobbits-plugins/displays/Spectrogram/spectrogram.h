#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "displayinterface.h"
#include "spectrogramcontrols.h"
#include "spectrogramwidget.h"

class Spectrogram : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.Spectrogram")
    Q_INTERFACES(DisplayInterface)

public:
    Spectrogram();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    SpectrogramWidget* m_displayWidget;
    SpectrogramControls* m_controlsWidget;
};

#endif // SPECTROGRAM_H
