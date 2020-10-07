#include "bitraster.h"

BitRaster::BitRaster() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* BitRaster::createDefaultDisplay()
{
    return new BitRaster();
}

QString BitRaster::name()
{
    return "Bit Raster";
}

QString BitRaster::description()
{
    return "Displays each bit in the data as a single colored pixel";
}

QStringList BitRaster::tags()
{
    return {"Generic"};
}

QWidget* BitRaster::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* BitRaster::controls(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void BitRaster::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new BitRasterWidget(displayHandle, this);
        m_controlsWidget = new BitRasterControls();

        connect(m_controlsWidget, SIGNAL(scaleSet(int)), m_displayWidget, SLOT(setScale(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
    }
}
