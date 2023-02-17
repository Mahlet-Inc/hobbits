#include "displayhandle.h"
#include <QScrollBar>
#include "displaywidget.h"

DisplayHandle::DisplayHandle(QSharedPointer<BitContainerManager> bitManager) :
    m_bitManager(bitManager),
    m_bitOffset(0),
    m_frameOffset(0),
    m_bitOffsetControl(nullptr),
    m_frameOffsetControl(nullptr),
    m_bitOffsetHover(-1),
    m_frameOffsetHover(-1)
{
    connect(
            m_bitManager.data(),
            SIGNAL(currSelectionChanged(QSharedPointer<BitContainer>, QSharedPointer<BitContainer>)),
            this,
            SLOT(checkCurrentContainer()));
}

QSharedPointer<BitContainer> DisplayHandle::currentContainer() const
{
    return m_bitManager->currentContainer();
}

qint64 DisplayHandle::bitOffset() const
{
    return m_bitOffset;
}

qint64 DisplayHandle::frameOffset() const
{
    return m_frameOffset;
}

QSet<DisplayWidget *> DisplayHandle::activeDisplays() const
{
    return m_activeDisplays;
}

void DisplayHandle::setFrameOffsetControl(QAbstractSlider *frameOffsetControl)
{
    if (m_frameOffsetControl) {
        disconnect(m_frameOffsetControl, SIGNAL(valueChanged(int)), this, SLOT(offsetControlUpdate()));
    }
    m_frameOffsetControl = frameOffsetControl;
    if (m_frameOffsetControl) {
        m_frameOffsetControl->setValue(int(m_frameOffset));
        connect(m_frameOffsetControl, SIGNAL(valueChanged(int)), this, SLOT(offsetControlUpdate()));
    }
    configureControls();
}

void DisplayHandle::setBitOffsetControl(QAbstractSlider *bitOffsetControl)
{
    if (m_bitOffsetControl) {
        disconnect(m_bitOffsetControl, SIGNAL(valueChanged(int)), this, SLOT(offsetControlUpdate()));
    }
    m_bitOffsetControl = bitOffsetControl;
    if (m_bitOffsetControl) {
        m_bitOffsetControl->setValue(int(m_bitOffset));
        connect(m_bitOffsetControl, SIGNAL(valueChanged(int)), this, SLOT(offsetControlUpdate()));
    }
    configureControls();
}

QAbstractSlider *DisplayHandle::frameOffsetControl()
{
    return m_frameOffsetControl;
}

QAbstractSlider *DisplayHandle::bitOffsetControl()
{
    return m_bitOffsetControl;
}

void DisplayHandle::setBitOffset(qint64 bitOffset)
{
    setOffsets(bitOffset, m_frameOffset);
}

void DisplayHandle::setFrameOffset(qint64 frameOffset)
{
    setOffsets(m_bitOffset, frameOffset);
}

void DisplayHandle::setOffsets(qint64 bitOffset, qint64 frameOffset)
{
    m_frameOffsetHover = -1;
    m_bitOffsetHover = -1;

    if (currentContainer().isNull()) {
        // reset offsets if there is no container
        bitOffset = 0;
        frameOffset = 0;
    }
    else if (bitOffset < 0
            || frameOffset < 0) {
        // ignore invalid offsets
        return;
    }
    else {
        // force offset within container limits
        if (currentContainer()->frameCount() > 0 && frameOffset >= currentContainer()->frameCount()) {
            frameOffset = currentContainer()->frameCount() - 1;
        }
        if (currentContainer()->maxFrameWidth() > 0 && bitOffset >= currentContainer()->maxFrameWidth()) {
            bitOffset = currentContainer()->maxFrameWidth() - 1;
        }
    }

    bool bitChanged = false;
    bool frameChanged = false;
    if (bitOffset != m_bitOffset) {
        m_bitOffset = bitOffset;
        bitChanged = true;
        if (m_bitOffsetControl) {
            m_bitOffsetControl->setValue(int(m_bitOffset));
        }
    }

    if (frameOffset != m_frameOffset) {
        m_frameOffset = frameOffset;
        frameChanged = true;
        if (m_frameOffsetControl) {
            m_frameOffsetControl->setValue(int(m_frameOffset));
        }
    }

    if (bitChanged || frameChanged) {
        emit newOffsets(m_bitOffset, m_frameOffset);
        if (bitChanged) {
            emit newBitOffset(m_bitOffset);
        }
        if (frameChanged) {
            emit newFrameOffset(m_frameOffset);
        }
        for (auto widget: m_activeDisplays) {
            setMouseHover(widget->display().data(), mouseHover(widget->display().data()));
        }
    }
}

void DisplayHandle::setMouseHover(DisplayInterface* display, QPoint mouseHover)
{
    m_mouseHoverMap.remove(display);
    m_mouseHoverMap.insert(display, mouseHover);
    emit newMouseHover(display, mouseHover);
}

void DisplayHandle::setBitHover(bool hovering, qint64 bitOffset, qint64 frameOffset)
{
    if (!hovering
            || frameOffset < 0
            || bitOffset < 0
            || currentContainer().isNull()) {
        m_frameOffsetHover = -1;
        m_bitOffsetHover = -1;
        setStatus("");
        emit newBitHover(m_bitOffsetHover, m_frameOffsetHover);
        return;
    }
    frameOffset += m_frameOffset;
    bitOffset += m_bitOffset;
    if (frameOffset >= currentContainer()->frameCount()
            || bitOffset >= currentContainer()->frameAt(frameOffset).size()) {
        m_frameOffsetHover = -1;
        m_bitOffsetHover = -1;
        setStatus("");
        emit newBitHover(m_bitOffsetHover, m_frameOffsetHover);
        return;
    }

    m_frameOffsetHover = frameOffset;
    m_bitOffsetHover = bitOffset;
    qint64 totalBitOffset = currentContainer()->frameAt(frameOffset).start() + bitOffset;
    qint64 totalByteOffset = totalBitOffset / 8;
    setStatus(
            QString("Bit Offset: %L1  Byte Offset: %L2  Frame Offset: %L3  Frame Bit Offset: %L4").arg(
                    totalBitOffset).arg(totalByteOffset).arg(frameOffset).arg(bitOffset));
    emit newBitHover(m_bitOffsetHover, m_frameOffsetHover);
}

void DisplayHandle::setStatus(QString status)
{
    emit newStatus(status);
}

void DisplayHandle::setActiveDisplays(QSet<DisplayWidget *> activeDisplays)
{
    m_activeDisplays = activeDisplays;
    configureControls();
    emit newActiveDisplays(activeDisplays);
}

void DisplayHandle::setRenderedRange(DisplayInterface *display, Range range)
{
    m_renderedRangeMap.insert(display, range);
    emit renderedRangeChanged(range);
}

void DisplayHandle::requestOverlayRedraw(DisplayInterface* display)
{
    emit overlayRedrawRequested(display);
}

void DisplayHandle::requestFullRedraw(DisplayInterface* display)
{
    emit fullRedrawRequested(display);
}

void DisplayHandle::offsetControlUpdate()
{
    qint64 bitOffset = m_bitOffset;
    qint64 frameOffset = m_frameOffset;

    if (m_bitOffsetControl) {
        bitOffset = m_bitOffsetControl->value();
    }

    if (m_frameOffsetControl) {
        frameOffset = m_frameOffsetControl->value();
    }

    setOffsets(bitOffset, frameOffset);
}

void DisplayHandle::configureControls()
{
    if (m_bitOffsetControl) {
        bool showBitOffset = false;
        for (auto display: m_activeDisplays) {
            if (!display->display()->renderConfig()->hideBitOffsetControls()) {
                showBitOffset = true;
                break;
            }
        }
        if (!showBitOffset || currentContainer().isNull() || currentContainer()->maxFrameWidth() < 1) {
            m_bitOffsetControl->setVisible(false);
        }
        else {
            m_bitOffsetControl->setVisible(true);
            m_bitOffsetControl->setMinimum(0);
            m_bitOffsetControl->setMaximum(int(currentContainer()->maxFrameWidth() - 1));
        }
    }

    if (m_frameOffsetControl) {
        bool showFrameOffset = false;
        for (auto display: m_activeDisplays) {
            if (!display->display()->renderConfig()->hideFrameOffsetControls()) {
                showFrameOffset = true;
                break;
            }
        }
        if (!showFrameOffset || currentContainer().isNull() || currentContainer()->frameCount() < 1) {
            m_frameOffsetControl->setVisible(false);
        }
        else {
            m_frameOffsetControl->setVisible(true);
            m_frameOffsetControl->setMinimum(0);
            m_frameOffsetControl->setMaximum(int(currentContainer()->frameCount() - 1));
        }
    }
}

void DisplayHandle::checkCurrentContainer()
{
    disconnect(this, SLOT(containerUpdate()));

    if (!currentContainer().isNull()) {
        connect(currentContainer().data(), SIGNAL(changed()), this, SLOT(containerUpdate()), Qt::QueuedConnection);
    }
    emit currentContainerChanged();

    containerUpdate();
}

void DisplayHandle::containerUpdate()
{
    configureControls();
    setOffsets(m_bitOffset, m_frameOffset);
    emit containerChanged();
}

QPoint DisplayHandle::mouseHover(DisplayInterface* display) const
{
    return m_mouseHoverMap.value(display);
}

Range DisplayHandle::renderedRange(DisplayInterface *display) const
{
    return m_renderedRangeMap.value(display);
}

void DisplayHandle::deactivate() 
{
    m_bitOffsetControl = nullptr;
    m_frameOffsetControl = nullptr;
}

qint64 DisplayHandle::bitOffsetHover() const
{
    return m_bitOffsetHover;
}

qint64 DisplayHandle::frameOffsetHover() const
{
    return m_frameOffsetHover;
}