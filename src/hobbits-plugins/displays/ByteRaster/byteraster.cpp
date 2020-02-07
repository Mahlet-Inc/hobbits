#include "byteraster.h"

ByteRaster::ByteRaster() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* ByteRaster::createDefaultDisplay()
{
    return new ByteRaster();
}

QString ByteRaster::getName()
{
    return "Byte Raster";
}

QWidget* ByteRaster::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* ByteRaster::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void ByteRaster::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new ByteRasterWidget(displayHandle, this);
        m_controlsWidget = new ByteRasterControls();

        connect(m_controlsWidget, SIGNAL(scaleSet(int)), m_displayWidget, SLOT(setScale(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
    }
}
