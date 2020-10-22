#include "metadata.h"

Metadata::Metadata() :
    m_displayWidget(nullptr)
{

}

DisplayInterface* Metadata::createDefaultDisplay()
{
    return new Metadata();
}

QString Metadata::name()
{
    return "Metadata";
}

QString Metadata::description()
{
    return "Displays bit container metadata";
}

QStringList Metadata::tags()
{
    return {"Generic"};
}

QWidget* Metadata::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* Metadata::controls(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return nullptr;
}

void Metadata::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new MetadataWidget(displayHandle, this);
    }
}
