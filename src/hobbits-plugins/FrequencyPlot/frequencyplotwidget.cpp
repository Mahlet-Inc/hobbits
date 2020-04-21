#include "frequencyplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"

FrequencyPlotWidget::FrequencyPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent)
{

}

void FrequencyPlotWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    QImage raster = DisplayHelper::getBitRasterImage(
                m_displayHandle->getContainer(),
                m_displayHandle->getBitOffset(),
                m_displayHandle->getFrameOffset(),
                this->width(), this->height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(0, 0, raster);
}

void FrequencyPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    sendHoverUpdate(event, 1, 1, 1, 1, QPoint(0, 0));
}


