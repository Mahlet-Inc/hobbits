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

QString ByteRaster::name()
{
    return "Byte Raster";
}

QString ByteRaster::description()
{
    return "Displays each byte in the data as a single colored pixel";
}

QStringList ByteRaster::tags()
{
    return {"Generic"};
}

QWidget* ByteRaster::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* ByteRaster::controls(QSharedPointer<DisplayHandle> displayHandle)
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
