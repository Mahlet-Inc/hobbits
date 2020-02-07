#include "symbolraster.h"

SymbolRaster::SymbolRaster() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* SymbolRaster::createDefaultDisplay()
{
    return new SymbolRaster();
}

QString SymbolRaster::getName()
{
    return "Symbol Raster";
}

QWidget* SymbolRaster::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* SymbolRaster::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void SymbolRaster::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new SymbolRasterWidget(displayHandle, this);
        m_controlsWidget = new SymbolRasterControls();

        connect(m_controlsWidget, SIGNAL(scaleSet(int)), m_displayWidget, SLOT(setScale(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
        connect(
                m_controlsWidget,
                SIGNAL(colorMappingChanged(QList<QPair<QString,QColor>>)),
                m_displayWidget,
                SLOT(setColorMapping(QList<QPair<QString,QColor>>)));

        m_controlsWidget->updateColorMap();
    }
}
